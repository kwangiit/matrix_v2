/*
 * scheduler_stub.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"
#include "matrix_epoll_server.h"
#include <math.h>
#include <algorithm>

MatrixScheduler::MatrixScheduler(const string
		&configFile): Peer(configFile)
{
	timespec start, end;
	clock_gettime(0, &start);

	/* number of neighbors is equal to the
	 * squared root of all number of schedulers
	 * */
	if (schedulerVec.size() == 1)
	{
		config->workStealingOn = 0;
	}

	numNeigh = (int)(sqrt(schedulerVec.size()));

	neighIdx = new int[numNeigh];
	maxLoadedIdx = -1;
	maxLoad = -1000000;
	pollInterval = config->wsPollIntervalStart;
	chooseBitMap = new bool[schedulerVec.size()];
	reset_choosebm();
	startWS = false;

	ZHTMsgCountMutex = Mutex();
	numIdleCoreMutex = Mutex();
	numTaskFinMutex = Mutex();

	wqMutex = Mutex();
	rqMutex = Mutex();
	cqMutex = Mutex();
	lqMutex = Mutex();
	wsqMutex = Mutex();
	ldMutex = Mutex();
	tteMutex = Mutex();

	clock_gettime(0, &end);

	if (config->schedulerLog == 1)
	{
		string schedulerLogFile("./scheduler_" + (num_to_str<int>(
			schedulerVec.size())) + "_" + num_to_str<long>(
			config->numTaskPerClient) + "_" + num_to_str<int>(get_index()));
		schedulerLogOS.open(schedulerLogFile.c_str());

		timespec diff = time_diff(start, end);
		schedulerLogOS << "I am a Matrix Scheduler, it takes me " <<
				diff.tv_sec << "s, and " << diff.tv_nsec <<
				" ns for initialization!" << endl;
	}

	numIdleCore = config->numCorePerExecutor;
	prevNumTaskFin = 0;
	numTaskFin = 0;
	numTaskSteal = 0;
	numTaskStolen = 0;
	numWS = 0;
	numWSFail = 0;

	waitQueue = deque<TaskMsg>();
	localQueue = priority_queue<TaskMsg, vector<TaskMsg>, HighPriorityByDataSize>();
	wsQueue = priority_queue<TaskMsg, vector<TaskMsg>, HighPriorityByDataSize>();
	completeQueue = deque<CmpQueueItem>();

	localData = map<string, string>();
	cache = false;
#ifdef DATA_CACHE
	cache = true;
#endif

	string taskLogFile("./task_" + (num_to_str<int>(
			schedulerVec.size())) + "_" + num_to_str<long>(
			config->numTaskPerClient) + "_" + num_to_str<int>(get_index()));
	taskLogOS.open(taskLogFile.c_str());
}

MatrixScheduler::~MatrixScheduler(void)
{

}

/* the scheduler tries to regist to ZHT server by increasing a counter.
 * The purpose of doing the registration is to ensure that all the
 * schedulers are running at the beginning before moving forward
 * */
void MatrixScheduler::regist()
{
	string regKey("number of scheduler registered");
	string taskFinKey("num tasks done");
	string recvKey("num tasks recv");

	/* the first scheduler (index = 0) intializes the records
	 * including both the number of registered schedulers and
	 * the number of tasks done
	 * */
	if (get_index() == 0)
	{
		insert_wrap(regKey, "1");
		insert_wrap(taskFinKey, "0");
		insert_wrap(recvKey, "0");
	}
	else
	{
		string value;
		lookup_wrap(regKey, value);
		while (value.empty())
		{
			usleep(config->sleepLength);
			lookup_wrap(regKey, value);
		}

		int newValNum = str_to_num<int>(value) + 1;
		string newVal = num_to_str<int>(newValNum);
		string queryVal;

		while (zc.compare_swap(regKey, value, newVal, queryVal) != 0)
		{
			if (queryVal.empty())
			{
				lookup_wrap(regKey, value);
			}
			else
			{
				value = queryVal;
			}
			newValNum = str_to_num<int>(value) + 1;
			newVal = num_to_str<int>(newValNum);
			usleep(config->sleepLength);
		}
	}
}

void MatrixScheduler::get_task_from_file()
{
	string done;

	zc.lookup("Split Workload", done);
	while (done.empty())
	{
		usleep(1000);
		zc.lookup("Split Workload", done);
	}

	string filePath = config->schedulerWorkloadPath + "/workload." + num_to_str<int>(get_index());
	string line;

	ifstream fileStream(filePath.c_str());
	if (!fileStream.good())
	{
		return;
	}
	else
	{
		int numTask = 0;
		while (getline(fileStream, line))
		{
			numTask++;
			vector<string> taskItemStr = tokenize(line, " ");
			TaskMsg tm;
			tm.set_taskid(taskItemStr.at(0));
			tm.set_user(taskItemStr.at(1));
			tm.set_dir(taskItemStr.at(2));
			tm.set_cmd(taskItemStr.at(3));
			tm.set_datalength(0);
			long time = get_time_usec();
			taskTimeEntry.push_back(tm.taskid() +
					"\tWaitQueueTime\t" + num_to_str<long>(time));
			waitQueue.push_back(tm);
		}

		string numTaskRecvStr, numTaskRecvMoreStr, queryValue;
		lookup_wrap("num tasks recv", numTaskRecvStr);

		long numTaskRecv = str_to_num<long>(numTaskRecvStr);
		numTaskRecv += numTask;
		numTaskRecvMoreStr = num_to_str<long>(numTaskRecv);
		//cout << "number of task more recv is:" << numTaskRecv << endl;
		while (zc.compare_swap("num tasks recv", numTaskRecvStr, numTaskRecvMoreStr, queryValue) != 0)
		{
			if (queryValue.empty())
			{
				lookup_wrap("num tasks recv", numTaskRecvStr);
			}
			else
			{
				numTaskRecvStr = queryValue;
			}
			//cout << "OK, conflict, current value is:" << numTaskRecvStr << endl;
			numTaskRecv = str_to_num<long>(numTaskRecvStr);
			numTaskRecv += numTask;
			numTaskRecvMoreStr = num_to_str<long>(numTaskRecv);
		}
	}
}

/* pack several tasks (numTask) together, and send them
 * with one package to another thief scheduler. The tasks
 * are delimited with "eot"
 * */
void MatrixScheduler::pack_send_task(
		int numTask, int sockfd, sockaddr fromAddr)
{
	MatrixMsg mmTasks;
	mmTasks.set_msgtype("scheduler send task");
	mmTasks.set_count(numTask);

	for (int j = 0; j < numTask; j++)
	{
		mmTasks.add_tasks(taskmsg_to_str(wsQueue.top()));
		wsQueue.pop();
	}

	string strTasks = mmTasks.SerializeAsString();
	send_bf(sockfd, strTasks);
}

/* send tasks to another thief scheduler */
void MatrixScheduler::send_task(int sockfd, sockaddr fromAddr)
{
	int numTaskToSend = -1;

	wsqMutex.lock();

	/* number of tasks to send equals to half of the current load,
	 * which is calculated as the number of tasks in the ready queue
	 * minus number of idle cores */
	numTaskToSend = wsQueue.size() / 2;

	//cout << "Number of task being stolen is:" << numTaskToSend << endl;
	MatrixMsg mmNumTask;
	mmNumTask.set_msgtype("scheduler send task number");
	mmNumTask.set_count(numTaskToSend);
	string strNumTask = mmNumTask.SerializeAsString();
	send_bf(sockfd, strNumTask);

	if (numTaskToSend > 0)
	{
		int numSend = numTaskToSend / config->maxTaskPerPkg;

		for (int i = 0; i < numSend; i++)
		{
			pack_send_task(config->maxTaskPerPkg, sockfd, fromAddr);
		}

		long numTaskLeft = numTaskToSend - numSend * config->maxTaskPerPkg;

		if (numTaskLeft > 0)
		{
			pack_send_task(numTaskLeft, sockfd, fromAddr);
		}
	}

	wsqMutex.unlock();
}

/* receive tasks submitted by client */
void MatrixScheduler::recv_task_from_client(
		MatrixMsg &mm, int sockfd, sockaddr fromAddr)
{
	long increment = 0;
	MatrixMsg mmNumTask;
	mmNumTask.set_msgtype("return to client");
	mmNumTask.set_count(mm.count());
	string numTaskStr = mmNumTask.SerializeAsString();

	send_bf(sockfd, numTaskStr);

	long sec = 0, nsec = 0;
	//cout << "Number of task received is:" << mm.count() << endl;
	for (int i = 0; i < mm.count(); i++)
	{
		//cout << "The task is:" << mm.tasks(i) << endl;
		TaskMsg tm = str_to_taskmsg(mm.tasks(i));
		//cout << "The id is:" << tm.taskid() << endl;
		/* update the task metadata in ZHT */
//		string taskDetail, taskDetailAttempt, queryValue;
//		lookup_wrap(tm.taskid(), taskDetail);
//
//		if (taskDetail.empty())
//		{
//			cout << "I am receiving from client, that is insane:" << tm.taskid() << endl;
//		}
//		Value value = str_to_value(taskDetail);
//		value.set_arrivetime(get_time_usec());
//		value.set_nummove(value.nummove() + 1);
//		value.set_history(value.history() + "|" + get_id());
//		taskDetailAttempt = value_to_str(value);
//
//		increment += 2;
//
//		while (zc.compare_swap(tm.taskid(), taskDetail, taskDetailAttempt, queryValue) != 0)
//		{
//			if (queryValue.empty())
//			{
//				lookup_wrap(tm.taskid(), taskDetail);
//				increment++;
//			}
//			else
//			{
//				taskDetail = queryValue;
//			}
//			value = str_to_value(taskDetail);
//			value.set_arrivetime(get_time_usec());
//			value.set_nummove(value.nummove() + 1);
//			value.set_history(value.history() + "|" + get_id());
//			taskDetailAttempt = value_to_str(value);
//			increment++;
//		}
		//wqMutex.lock();
		waitQueue.push_back(tm);

		//wqMutex.unlock();
		//cout << "task " << tm.taskid() << " has been received!" << endl;
	}

	string numTaskRecvStr, numTaskRecvMoreStr, queryValue;
	lookup_wrap("num tasks recv", numTaskRecvStr);
	long numTaskRecv = str_to_num<long>(numTaskRecvStr);
	numTaskRecv += mm.count();
	numTaskRecvMoreStr = num_to_str<long>(numTaskRecv);
	increment += 2;
	//cout << "number of task more recv is:" << numTaskRecv << endl;
	while (zc.compare_swap("num tasks recv", numTaskRecvStr, numTaskRecvMoreStr, queryValue) != 0)
	{
		if (queryValue.empty())
		{
			lookup_wrap("num tasks recv", numTaskRecvStr);
			increment++;
		}
		else
		{
			numTaskRecvStr = queryValue;
		}
		//cout << "OK, conflict, current value is:" << numTaskRecvStr << endl;
		numTaskRecv = str_to_num<long>(numTaskRecvStr);
		numTaskRecv += mm.count();
		numTaskRecvMoreStr = num_to_str<long>(numTaskRecv);
	}

	if (increment > 0)
	{
		//ZHTMsgCountMutex.lock();
		incre_ZHT_msg_count(increment);
		//ZHTMsgCountMutex.unlock();
	}
	//cout << "I am done with receiving tasks!" << endl;
}

void MatrixScheduler::recv_pushing_task(MatrixMsg &mm, int sockfd, sockaddr fromAddr)
{
	long increment = 0;
	TaskMsg tm = str_to_taskmsg(mm.tasks(0));

	/*string taskDetail;
	lookup_wrap(tm.taskid(), taskDetail);
	Value value = str_to_value(taskDetail);
	value.set_rqueuedtime(get_time_usec());
	value.set_nummove(value.nummove() + 1);
	value.set_history(value.history() + "|" + get_id());
	taskDetail = value_to_str(value);
	insert_wrap(tm.taskid(), taskDetail);*/

	tteMutex.lock();
	taskTimeEntry.push_back(tm.taskid() + "\tPushQueuedTime\t"
			+ num_to_str<long>(get_time_usec()));
	tteMutex.unlock();

	lqMutex.lock();
	localQueue.push(tm);
	lqMutex.unlock();
	//increment += 2;

	MatrixMsg mmSuc;
	mmSuc.set_msgtype("success receiving pushing task");
	string mmSucStr = mmSuc.SerializeAsString();
	send_bf(sockfd, mmSucStr);

	/*ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();*/
}

/* processing requests received by the epoll server */
int MatrixScheduler::proc_req(int sockfd, char *buf, sockaddr fromAddr)
{
	MatrixMsg mm;
	//string *sbuf = static_cast<string*>(buf);
	//string bufStr = *sbuf;

	//delete sbuf;
	mm.ParseFromArray(buf, strlen(buf));

	long increment = 0;

	/* message type is stored in pkg.virtualpath(), and contents
	 * are stored in pkg.readfullpath() */
	string msg = mm.msgtype();
	if (msg.compare("query load") == 0)	// thief quering load
	{
		int load = wsQueue.size();
		MatrixMsg mmLoad;
		mmLoad.set_msgtype("send load");
		mmLoad.set_count(load);
		string strLoad;
		strLoad = mmLoad.SerializeAsString();
		send_bf(sockfd, strLoad);
	}
	else if (msg.compare("steal task") == 0)	// thief steals tasks
	{
		send_task(sockfd, fromAddr);
	}
	else if (msg.compare("client send task") == 0)	// client sent tasks
	{
		/* add tasks and then send ack back */
		recv_task_from_client(mm, sockfd, fromAddr);
	}
	else if (msg.compare("scheduler push task") == 0)
	{
		recv_pushing_task(mm, sockfd, fromAddr);
	}
	else if (msg.compare("scheduler require data") == 0)
	{
		//cout << "The required information is" << mm.extrainfo() << endl;
		string dataPiece;
		ldMutex.lock();

		if (localData.find(mm.extrainfo()) == localData.end())
		{
			//cout << "What is the hell!" << endl;
			dataPiece = "shit, that is wrong!";
		}
		else
		{
			dataPiece = localData.find(mm.extrainfo())->second;
		}
		ldMutex.unlock();

		MatrixMsg mmDataPiece;
		mmDataPiece.set_msgtype("scheduler send data");
		mmDataPiece.set_extrainfo(dataPiece);
		string dataStr = mmDataPiece.SerializeAsString();
		send_bf(sockfd, dataStr);
	}
	return 1;
}

/* epoll server thread function */
void *epoll_serving(void *args)
{
	MatrixEpollServer *mes = (MatrixEpollServer*)args;
	mes->serve();
	pthread_exit(NULL);
	return NULL;
}

/* fork epoll server thread */
void MatrixScheduler::fork_es_thread()
{
	MatrixEpollServer *mes = new MatrixEpollServer(config->schedulerPortNo, this);

	pthread_t esThread;

	while (pthread_create(&esThread, NULL, epoll_serving, (void*)mes) != 0)
	{
		sleep(1);
	}
}

/* reset the bitmap of neighbors chosen, "false"
 * means hasn't been chosen */
void MatrixScheduler::reset_choosebm()
{
	for (int i = 0; i < schedulerVec.size(); i++)
	{
		chooseBitMap[i] = false;
	}
}

/* choose candidate neighbors to steal tasks,
 * for simplicity, we randomly choose neighbors
 * */
void MatrixScheduler::choose_neigh()
{
	srand(time(NULL));
	int idx = -1;
	for (int i = 0; i < numNeigh; i++)
	{
		idx = rand() % schedulerVec.size();
		while (idx == get_index() || chooseBitMap[idx])
		{
			idx = rand() % schedulerVec.size();
		}
		neighIdx[i] = idx;
		chooseBitMap[idx] = true;
	}
	reset_choosebm();
}

/* find the neighbor with the maximum load by quering
 * the load information of each scheduler one by one
 * */
void MatrixScheduler::find_most_loaded_neigh()
{
	MatrixMsg mm;
	mm.set_msgtype("query load");

	string strLoadQuery = mm.SerializeAsString();

	long load = -1;

	for (int i = 0; i < numNeigh; i++)
	{
		string result;
		int sockfd = send_first(schedulerVec.at(neighIdx[i]),
				config->schedulerPortNo, strLoadQuery);
		recv_bf(sockfd, result);
		close(sockfd);
		MatrixMsg mmLoad;
		mmLoad.ParseFromString(result);

		load = mmLoad.count();

		if (maxLoad < load)
		{
			maxLoad = load;
			maxLoadedIdx = neighIdx[i];
		}
	}
}

/* receive several tasks (numTask) from another scheduler */
void MatrixScheduler::recv_task_from_scheduler(int sockfd, long numTask)
{
	/* compute how many receives needed as there is a limit of
	 * maximum number of tasks that can be sent once
	 * */
	long numRecv = numTask / config->maxTaskPerPkg;
	if (numRecv * config->maxTaskPerPkg < numTask)
	{
		numRecv++;
	}

	//long increment = 0;

	for (long i = 0; i < numRecv; i++)
	{
		string taskPkgStr;
		recv_bf(sockfd, taskPkgStr);
		MatrixMsg mm;
		mm.ParseFromString(taskPkgStr);


		vector<TaskMsg> tmVec;
		string time = num_to_str<long>(get_time_usec());

		for (long j = 0; j < mm.count(); j++)
		{
			tmVec.push_back(str_to_taskmsg(mm.tasks(j)));
		}

		tteMutex.lock();
		for (long j = 0; j < mm.count(); j++)
		{
			taskTimeEntry.push_back(tmVec.at(j).taskid() +
					"\tWorkStealQueuedTime\t" + time);
		}
		tteMutex.unlock();

		wsqMutex.lock();
		for (long j = 0; j < mm.count(); j++)
		{
			//TaskMsg tm = str_to_taskmsg(mm.tasks(j));
			/* update task metadata */
			/*string taskDetailStr;
			lookup_wrap(tm.taskid(), taskDetailStr);
			Value value = str_to_value(taskDetailStr);
			value.set_nummove(value.nummove() + 1);
			value.set_history(value.history() + "|" + get_id());
			value.set_rqueuedtime(get_time_usec());
			taskDetailStr = value_to_str(value);
			insert_wrap(tm.taskid(), taskDetailStr);*/
			wsQueue.push(tmVec.at(j));
			//increment += 2;
			//cout << "taskid is:" << tm.taskid() << ", description is:" << taskDetailStr << endl;
		}
		wsqMutex.unlock();
	}

	/*if (increment > 0)
	{
		ZHTMsgCountMutex.lock();
		incre_ZHT_msg_count(increment);
		ZHTMsgCountMutex.unlock();
	}*/
}

/* try to steal tasks from the most-loaded neighbor. The thief first
 * sends a message ("steal tasks") to the neighbor, and then waits
 * for the neighbor's response. The neighbor first sends a message
 * notifying how many tasks could be migrated, then sends all the
 * tasks batch by batch.
 * */
bool MatrixScheduler::steal_task()
{
	/* if no neighbors have ready tasks */
	if (maxLoad <= 0)
	{
		return false;
	}

	MatrixMsg mm;
	mm.set_msgtype("steal task");
	string strStealTask = mm.SerializeAsString();

	string numTaskPkgStr;
	int sockfd = send_first(schedulerVec.at(maxLoadedIdx), config->schedulerPortNo, strStealTask);
	recv_bf(sockfd, numTaskPkgStr);
	close(sockfd);
	MatrixMsg mmNumTask;
	mmNumTask.ParseFromString(numTaskPkgStr);

	int numTask = mmNumTask.count();

	//cout << "Number of task stolen is:" << numTask << endl;
	/* if the victim doesn't have tasks to migrate*/
	if (numTask == 0)
	{
		return false;
	}

	/* otherwise, receive numTask tasks from the victim*/
	recv_task_from_scheduler(sockfd, numTask);
	return true;
}

/* work stealing threading function, under the condition that the scheduler
 * is still processing tasks, as long as the ready queue is empty and the
 * poll interval has reached the upper bound, the scheduler would do work
 * stealing.
 * */
void *workstealing(void* args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	long incre = 0;

	while (ms->running)
	{
		//cout << "Now, start to do work stealing!" << endl;
		while (ms->localQueue.size() + ms->wsQueue.size() == 0 &&
				ms->pollInterval < ms->config->wsPollIntervalUb)
		{
			ms->choose_neigh();
			ms->find_most_loaded_neigh();
			bool success = ms->steal_task();
			ms->numWS++;
			ms->maxLoadedIdx = -1;
			ms->maxLoad = -1000000;

			/* if successfully steals some tasks, then the poll
			 * interval is set back to the initial value, otherwise
			 * sleep the poll interval length, and double the poll
			 * interval, and tries to do work stealing again
			 * */
			if (success)
			{
				ms->pollInterval = ms->config->wsPollIntervalStart;
			}
			else
			{
				ms->numWSFail++;
				usleep(ms->pollInterval);
				ms->pollInterval *= 2;
			}
		}

		if (ms->pollInterval >= ms->config->wsPollIntervalUb)
		{
			break;
		}

		ms->pollInterval = ms->config->wsPollIntervalStart;
	}

	ms->ZHTMsgCountMutex.lock();
	ms->incre_ZHT_msg_count(incre);
	ms->ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

/* fork work stealing thread */
void MatrixScheduler::fork_ws_thread()
{
	if (config->workStealingOn == 1)
	{
		pthread_t wsThread;
		while (pthread_create(&wsThread, NULL, workstealing, this))
		{
			sleep(1);
		}
	}
}

/* executing a task. A task's specification has several fields:
 * taskId, users, directory, command and arguments. They are
 * delimited with space. After a task is done, move it to the
 * complete queue.
 * */
void MatrixScheduler::exec_a_task(TaskMsg &tm)
{
	string taskDetail;
	lookup_wrap(tm.taskid(), taskDetail);
	Value value = str_to_value(taskDetail);

	long startTime = get_time_usec();

	string data("");

#ifdef ZHT_STORAGE
	string dataPiece;
	for (int i = 0; i < value.parents_size(); i++)
	{
		lookup_wrap(value.datanamelist(i), dataPiece);
		data += dataPiece;
	}
#else
	for (int i = 0; i < value.parents_size(); i++)
	{
		if (value.datasize(i) > 0)
		{
			if (value.parents(i).compare(get_id()) == 0)
			{
				ldMutex.lock();
				data += localData.find(value.datanamelist(i))->second;
				//cout << tm.taskid() << " find the data" << endl;
				ldMutex.unlock();
			}
			else
			{
				bool dataReq = true;
				if (cache)
				{
					ldMutex.lock();
					if (localData.find(value.datanamelist(i)) != localData.end())
					{
						data += localData.find(value.datanamelist(i))->second;
						dataReq = false;
					}
					else
					{
						dataReq = true;
					}
					ldMutex.unlock();
				}
				if (dataReq)
				{
					MatrixMsg mm;
					mm.set_msgtype("scheduler require data");
					//cout << "The require data is:" << value.datanamelist(i) << endl;
					mm.set_extrainfo(value.datanamelist(i));
					string mmStr;
					mmStr = mm.SerializeAsString();
					cout << tm.taskid() << "\trequires " << i << "\tdata!" << endl;

					timespec before, after, diff;
					clock_gettime(0, &before);
					int sockfd = send_first(value.parents(i), config->schedulerPortNo, mmStr);
					clock_gettime(0, &after);
					diff = time_diff(before, after);
					cout << tm.taskid() << "\tit takes " << diff.tv_sec << "s, and " << diff.tv_nsec
							<< "ns to send the " << i << "\tdata to scheduler " << value.parents(i) << endl;

					string dataPiece;
					clock_gettime(0, &before);
					recv_bf(sockfd, dataPiece);
					clock_gettime(0, &after);
					close(sockfd);
					diff = time_diff(before, after);
					cout << tm.taskid() << "\tit takes " << diff.tv_sec << "s, and " << diff.tv_nsec
							<< "ns to receive the " << i << "\tdata from scheduler " << value.parents(i) << endl;
					MatrixMsg mmData;
					//cout << "The data piece is:" << dataPiece << ", task id is:" << tm.taskid() << ", before pasre!" << endl;
					mmData.ParseFromString(dataPiece);
					//cout << "After parse, extra info is:" << mmData.extrainfo() << endl;
					data += mmData.extrainfo();
					if (cache)
					{
						ldMutex.lock();
						localData.insert(make_pair(value.datanamelist(i), mmData.extrainfo()));
						ldMutex.unlock();
					}
				}
			}
		}
	}
#endif

	cout << tm.taskid() << "\tnow I received all the data" << endl;
	const char *execmd = tm.cmd().c_str();
	//cout << "The cmd is:" << execmd << endl;
	string result = exec(execmd);
	string key = get_id() + tm.taskid();

#ifdef ZHT_STORAGE
	insert_wrap(key, result);
#else
	ldMutex.lock();
	localData.insert(make_pair(key, result));
	//cout << "key is:" << key << ", and value is:" << result << endl;
	ldMutex.unlock();
#endif

	long finTime = get_time_usec();
	//taskDetail = value_to_str(value);
	//insert_wrap(tm.taskid(), taskDetail);

	tteMutex.lock();
	taskTimeEntry.push_back(tm.taskid() + "\tStartTime\t" +
			num_to_str<long>(startTime));
	taskTimeEntry.push_back(tm.taskid() + "\tFinTime\t" +
			num_to_str<long>(finTime));
	tteMutex.unlock();

	cqMutex.lock();
	completeQueue.push_back(CmpQueueItem(tm.taskid(), key, result.length()));
	cqMutex.unlock();

	numTaskFinMutex.lock();
	numTaskFin++;
	cout << tm.taskid() << "\tNumber of task fin is:" << numTaskFin << endl;
	numTaskFinMutex.unlock();

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(1);
	ZHTMsgCountMutex.unlock();
}

/* executing task thread function, under the conditin that the
 * scheduler is still processing tasks, as long as there are
 * tasks in the ready queue, execute the task one by one
 * */
void *executing_task(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	TaskMsg tm;

	while (ms->running)
	{
		while (ms->localQueue.size() > 0 || ms->wsQueue.size() > 0)
		{
			if (ms->localQueue.size() > 0)
			{
				ms->lqMutex.lock();
				if (ms->localQueue.size() > 0)
				{
					tm = ms->localQueue.top();
					ms->localQueue.pop();
					ms->lqMutex.unlock();
				}
				else
				{
					ms->lqMutex.unlock();
					continue;
				}
			}
			else if (ms->wsQueue.size() > 0)
			{
				ms->wsqMutex.lock();
				if (ms->wsQueue.size() > 0)
				{
					//cout << "The ready queue length is:" << ms->wsQueue.size() << endl;
					tm = ms->wsQueue.top();
					ms->wsQueue.pop();
					ms->wsqMutex.unlock();
				}
				else
				{
					ms->wsqMutex.unlock();
					continue;
				}
			}
			else
			{
				continue;
			}

			ms->numIdleCoreMutex.lock();
			ms->numIdleCore--;
			ms->numIdleCoreMutex.unlock();

			cout << "The task to execute is:" << tm.taskid() << endl;
			ms->exec_a_task(tm);

			ms->numIdleCoreMutex.lock();
			ms->numIdleCore++;
			ms->numIdleCoreMutex.unlock();
		}
	}

	pthread_exit(NULL);
	return NULL;
}

/* forking execute task threads. The number of executing threads is
 * given by the configuration file, and it is usually eaqual to the
 * number of cores a machine has.
 * */
void MatrixScheduler::fork_exec_task_thread()
{
	pthread_t *execThread = new pthread_t[config->numCorePerExecutor];

	for (int i = 0; i < config->numCorePerExecutor; i++)
	{
		while (pthread_create(&execThread[i], NULL, executing_task, this) != 0)
		{
			sleep(1);
		}
	}
}

int MatrixScheduler::task_ready_process(
		const Value &valuePkg, TaskMsg &tm)
{
	/* flag = 0, keep it in the work stealing queue
	 * flag = 1, keep it in the local queue
	 * flag = 2, push it to other scheduler's local queue
	 * */
	int flag = 2;

#ifdef ZHT_STORAGE
	tm.set_datalength(valuePkg.alldatasize());
	flag = 0;
#else
	if (valuePkg.alldatasize() <= config->dataSizeThreshold)
	{
		tm.set_datalength(valuePkg.alldatasize());
		flag = 0;
	}
	else
	{
		long maxDataSize = -1000000;
		string maxDataScheduler, key;
		for (int i = 0; i < valuePkg.datasize_size(); i++)
		{
			if (valuePkg.datasize(i) > maxDataSize)
			{
				maxDataSize = valuePkg.datasize(i);
				maxDataScheduler = valuePkg.parents(i);
				key = valuePkg.datanamelist(i);
			}
		}
		tm.set_datalength(maxDataSize);
		if (maxDataScheduler.compare(get_id()) == 0)
		{
			flag = 1;
		}
		else
		{
			bool taskPush = true;
			if (cache)
			{
				ldMutex.lock();
				if (localData.find(key) != localData.end())
				{
					flag = 1;
					taskPush = false;
				}
				else
				{
					taskPush = true;
				}
				ldMutex.unlock();
			}
			if (taskPush)
			{
				MatrixMsg mm;
				mm.set_msgtype("scheduler push task");
				mm.set_count(1);
				mm.add_tasks(taskmsg_to_str(tm));
				string mmStr = mm.SerializeAsString();
				int sockfd = send_first(maxDataScheduler, config->schedulerPortNo, mmStr);
				string ack;
				recv_bf(sockfd, ack);
				close(sockfd);
				flag = 2;
			}
		}
	}
#endif

	return flag;
}
/* check to see whether a task is ready to run or not. A task is
 * ready only if all of its parants are done (the indegree counter
 * equals to 0).
 * */
bool MatrixScheduler::check_a_ready_task(TaskMsg &tm)
{
	string taskDetail;

	bool ready = false;

	lookup_wrap(tm.taskid(), taskDetail);
	//cout << "task detail is:" << taskDetail << endl;
	if (taskDetail.empty())
	{
		cout << "that is insane:" << tm.taskid() << endl;
	}
	Value valuePkg = str_to_value(taskDetail);
	//cout << "task indegree:" << tm.taskid() << "\t" << valuePkg.indegree() << endl;
	if (valuePkg.indegree() == 0)
	{
		ready = true;
		int flag = task_ready_process(valuePkg, tm);
		if (flag != 2)
		{
			/*valuePkg.set_rqueuedtime(get_time_usec());
			taskDetail = value_to_str(valuePkg);
			insert_wrap(tm.taskid(), taskDetail);
			incre++;*/
			tteMutex.lock();
			taskTimeEntry.push_back(tm.taskid() + "\tReadyQueuedTime\t"
					+ num_to_str<long>(get_time_usec()));
			tteMutex.unlock();

		}
		if (flag == 0)
		{
			wsqMutex.lock();
			wsQueue.push(tm);
			cout << "The task that is ready is:" << tm.taskid() << endl;
			wsqMutex.unlock();
		}
		else if (flag == 1)
		{
			lqMutex.lock();
			localQueue.push(tm);
			cout << "The task that is ready is:" << tm.taskid() << endl;
			lqMutex.unlock();
		}
	}

	return ready;
}

/* checking ready task thread function, under the condition
 * that the scheduler is still processing tasks, if the
 * waiting queue is not empty, check all the tasks in the
 * waiting queue to see it they are ready to run. Move the
 * tasks that are ready to run to the ready queue.
 * */
void *checking_ready_task(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	TaskMsg tm;
	long increment = 0;

	while (ms->running)
	{
		while (ms->waitQueue.size() > 0)
		{
			ms->wqMutex.lock();
			if (ms->waitQueue.size() > 0)
			{
				//cout << "number of task waiting is:" << ms->waitQueue.size() << endl;
				tm = ms->waitQueue.front();
				ms->waitQueue.pop_front();
				//cout << "next one to process is:" << tm.taskid() << endl;
				ms->wqMutex.unlock();
			}
			else
			{
				ms->wqMutex.unlock();
				continue;
			}

			bool ready = ms->check_a_ready_task(tm);
			increment++;
			if (!ready)
			{
				ms->wqMutex.lock();
				ms->waitQueue.push_back(tm);
				//cout << "Ok, the task is still not ready!" << tm.taskid() << endl;
				ms->wqMutex.unlock();
			}
		}
	}

	ms->ZHTMsgCountMutex.lock();
	ms->incre_ZHT_msg_count(increment);
	ms->ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

/* fork check ready task thread */
void MatrixScheduler::fork_crt_thread()
{
	pthread_t crtThread;
	//cout << "The number of waiting task is:" << waitQueue.size() << endl;
	//cout << "The first one is:" << waitQueue.front().taskid() << endl;
	while (pthread_create(&crtThread, NULL, checking_ready_task, this))
	{
		sleep(1);
	}
}

/* decrease the indegree of a task by one, because one of
 * its parents has been done.
 * */
long MatrixScheduler::notify_children(const CmpQueueItem &cqItem)
{
	string taskDetail;
	long increment = 0;
	lookup_wrap(cqItem.taskId, taskDetail);
	if (taskDetail.empty())
	{
		cout << "I am notifying a children, that is insane:" << cqItem.taskId << endl;
	}
	Value value = str_to_value(taskDetail);

	increment++;
	string childTaskId, childTaskDetail, childTaskDetailAttempt, query_value;
	Value childVal;

	cout << "task finished, notify children:" << cqItem.taskId << "\t" << taskDetail << endl;
	for (int i = 0; i < value.children_size(); i++)
	{
		childTaskId = value.children(i);
		lookup_wrap(childTaskId, childTaskDetail);
		increment++;
		if (taskDetail.empty())
		{
			cout << "I am notifying a children, that is insane:" << cqItem.taskId << endl;
		}
		childVal = str_to_value(childTaskDetail);
		childVal.set_indegree(childVal.indegree() - 1);
		childVal.add_parents(get_id());
		childVal.add_datanamelist(cqItem.key);
		childVal.add_datasize(cqItem.dataSize);
		childVal.set_alldatasize(childVal.alldatasize() + cqItem.dataSize);
		childTaskDetailAttempt = value_to_str(childVal);

		increment++;
		while (zc.compare_swap(childTaskId, childTaskDetail, childTaskDetailAttempt, query_value) != 0)
		{
			if (query_value.empty())
			{
				lookup_wrap(childTaskId, childTaskDetail);
				increment++;
			}
			else
			{
				childTaskDetail = query_value;
			}
			childVal = str_to_value(childTaskDetail);
			childVal.set_indegree(childVal.indegree() - 1);
			childVal.add_parents(get_id());
			childVal.add_datanamelist(cqItem.key);
			childVal.add_datasize(cqItem.dataSize);
			childVal.set_alldatasize(childVal.alldatasize() + cqItem.dataSize);
			childTaskDetailAttempt = value_to_str(childVal);
			increment++;
		}
	}

	return increment;
}

/* checking complete queue tasks thread function, under the condition
 * that the scheduler is still processing tasks, as long as the task
 * complete queue is not empty, for each task in the queue, decrease
 * the indegree of each child by one.
 * */
void *checking_complete_task(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	CmpQueueItem cqItem = CmpQueueItem();

	long increment = 0;

	while (ms->running)
	{
		while (ms->completeQueue.size() > 0)
		{
			ms->cqMutex.lock();
			if (ms->completeQueue.size() > 0)
			{
				cqItem = ms->completeQueue.front();
				ms->completeQueue.pop_front();
				ms->cqMutex.unlock();
			}
			else
			{
				ms->cqMutex.unlock();
				continue;
			}
			increment += ms->notify_children(cqItem);
		}
	}

	ms->ZHTMsgCountMutex.lock();
	ms->incre_ZHT_msg_count(increment);
	ms->ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

/* fork check complete queue tasks thread */
void MatrixScheduler::fork_cct_thread()
{
	pthread_t cctThread;

	while (pthread_create(&cctThread, NULL, checking_complete_task, this))
	{
		sleep(1);
	}
}

/* recording status thread function. The recording thread would periodically
 * dump the scheduler status information (number of tasks done, waiting,
 * and ready; number of idle/all cores, and number of (failed) working
 * stealing operations) to ZHT.
 * */
void *recording_stat(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	long increment = 0;
	timespec time;

	if (ms->schedulerLogOS.is_open())
	{
		ms->schedulerLogOS << "Time\tNumTaskFin\tNumTaskWait\tNumTaskReady\t"
				"NumIdleCore\tNumAllCore\tNumWorkSteal\tNumWorkStealFail" << endl;
	}

	while (1)
	{
		Value recordVal;
		recordVal.set_id(ms->get_id());
		recordVal.set_numtaskfin(ms->numTaskFin);
		recordVal.set_numtaskwait(ms->waitQueue.size());
		recordVal.set_numtaskready(ms->localQueue.size() + ms->wsQueue.size());
		recordVal.set_numcoreavilable(ms->numIdleCore);
		recordVal.set_numallcore(ms->config->numCorePerExecutor);
		recordVal.set_numworksteal(ms->numWS);
		recordVal.set_numworkstealfail(ms->numWSFail);
		string recordValStr = value_to_str(recordVal);
		ms->insert_wrap(ms->get_id(), recordValStr);

		if (ms->schedulerLogOS.is_open())
		{
			ms->schedulerLogOS << get_time_usec() << "\t" << ms->numTaskFin << "\t" <<
					ms->waitQueue.size() << "\t" << ms->localQueue.size() + ms->wsQueue.size() <<
					"\t" << ms->numIdleCore << "\t" << ms->config->numCorePerExecutor << "\t" <<
					ms->numWS << "\t" << ms->numWSFail << endl;
		}

		/* check and modify how many tasks are done for all the schedulers. If all
		 * tasks are done, then flipping the scheduler status to off to indicate
		 * that it is not processing any task any more
		 * */
		string key("num tasks done");
		string numTaskDoneStr;
		ms->lookup_wrap(key, numTaskDoneStr);

		increment += 2;

		long numTaskDone = str_to_num<long>(numTaskDoneStr);
		if (numTaskDone == ms->config->numAllTask)
		{
			if (ms->schedulerLogOS.is_open())
			{
				ms->schedulerLogOS << get_time_usec() << "\t" << ms->numTaskFin << "\t" <<
						ms->waitQueue.size() << "\t" << ms->localQueue.size() + ms->wsQueue.size()
						<< "\t" << ms->numIdleCore << "\t" << ms->config->numCorePerExecutor <<
						"\t" << ms->numWS << "\t" << ms->numWSFail << endl;

			}
			ms->running = false;
			break;
		}

		ms->numTaskFinMutex.lock();

		numTaskDone += (ms->numTaskFin - ms->prevNumTaskFin);
		string numTaskDoneStrNew = num_to_str<long>(numTaskDone);
		string queryValue;
		increment++;
		while (ms->zc.compare_swap(key, numTaskDoneStr,
				numTaskDoneStrNew, queryValue) != 0)
		{
			if (queryValue.empty())
			{
				ms->lookup_wrap(key, numTaskDoneStr);
				increment++;
			}
			else
			{
				numTaskDoneStr = queryValue;
			}
			numTaskDone = str_to_num<long>(numTaskDoneStr);
			if (numTaskDone == ms->config->numAllTask)
			{
				break;
			}
			numTaskDone += (ms->numTaskFin - ms->prevNumTaskFin);
			numTaskDoneStrNew = num_to_str<long>(numTaskDone);
			increment++;
		}
		ms->prevNumTaskFin = ms->numTaskFin;

		ms->numTaskFinMutex.unlock();
		usleep(ms->config->sleepLength);
	}

	ms->ZHTMsgCountMutex.lock();
	ms->incre_ZHT_msg_count(increment);
	ms->ZHTMsgCountMutex.unlock();

	ms->schedulerLogOS << "The number of ZHT message is:" << ms->numZHTMsg << endl;
	ms->schedulerLogOS.flush(); ms->schedulerLogOS.close();

	if (ms->taskTimeEntry.size() > 0)
	{
		//ms->tteMutex.lock();
		for (int i = 0; i < ms->taskTimeEntry.size(); i++)
		{
			ms->taskLogOS << ms->taskTimeEntry.at(i) << endl;
		}
		//ms->tteMutex.unlock();
	}

	pthread_exit(NULL);
	return NULL;
}

/* fork recording status thread */
void MatrixScheduler::fork_record_stat_thread()
{
	pthread_t rsThread;

	while (pthread_create(&rsThread, NULL, recording_stat, this) != 0)
	{
		sleep(1);
	}
}

void *record_task_time(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;

	while (ms->running)
	{
		if (ms->taskTimeEntry.size() > 0)
		{
			ms->tteMutex.lock();
			while (ms->taskTimeEntry.size() > 0)
			{
				ms->taskLogOS << ms->taskTimeEntry.back() << endl;
				ms->taskTimeEntry.pop_back();
			}
			ms->tteMutex.unlock();
		}

		usleep(ms->config->sleepLength);
	}

	ms->taskLogOS.flush(); ms->taskLogOS.close();
	pthread_exit(NULL);

	return NULL;
}

void MatrixScheduler::fork_record_task_thread()
{
	pthread_t trThread;

	while (pthread_create(&trThread, NULL, record_task_time, this) != 0)
	{
		sleep(1);
	}
}

CmpQueueItem::CmpQueueItem(const string &taskId,
		const string &key, long dataSize)
{
	this->taskId = taskId;
	this->key = key;
	this->dataSize = dataSize;
}

CmpQueueItem::CmpQueueItem()
{
	this->taskId = "";
	this->key = "";
	this->dataSize = 0;
}

CmpQueueItem::~CmpQueueItem()
{

}
