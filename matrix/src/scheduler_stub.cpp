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
	numNeigh = (int)(sqrt(schedulerVec.size()));

	neighIdx = new int[numNeigh];
	maxLoadedIdx = -1;
	maxLoad = -1000000;
	pollInterval = config->wsPollIntervalStart;
	chooseBitMap = new bool[schedulerVec.size()];
	reset_choosebm();

	ZHTMsgCountMutex = Mutex();
	numIdleCoreMutex = Mutex();
	numTaskFinMutex = Mutex();

	wqMutex = Mutex();
	rqMutex = Mutex();
	cqMutex = Mutex();
	lqMutex = Mutex();
	wsqMutex = Mutex();
	ldMutex = Mutex();

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
	long increment = 0;

	/* the first scheduler (index = 0) intializes the records
	 * including both the number of registered schedulers and
	 * the number of tasks done
	 * */
	if (get_index() == 0)
	{
		zc.insert(regKey, "1");
		zc.insert(taskFinKey, "0");
		increment += 2;
	}
	else
	{
		string value;
		zc.lookup(regKey, value);
		increment++;
		while (value.empty())
		{
			usleep(config->sleepLength);
			zc.lookup(regKey, value);
			increment++;
		}

		int newValNum = str_to_num<int>(value) + 1;
		string newVal = num_to_str<int>(newValNum);
		string queryVal;
		increment++;

		while (zc.compare_swap(regKey, value, newVal, queryVal) != 0)
		{
			value = queryVal;
			newValNum = str_to_num<int>(value) + 1;
			newVal = num_to_str<int>(newValNum);
			increment++;
			usleep(config->sleepLength);
		}
	}

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();
}

/* pack several tasks (numTask) together, and send them
 * with one package to another thief scheduler. The tasks
 * are delimited with "eot"
 * */
void MatrixScheduler::pack_send_task(
		int numTask, int sockfd, sockaddr fromAddr)
{
	MatrixMsg mmTasks;
	mmTasks.set_count(numTask);

	for (int j = 0; j < numTask; j++)
	{
		MatrixMsg_TaskMsg *tm = mmTasks.add_tasks();
		*tm = wsQueue.top();
		wsQueue.pop();
	}

	string strTasks = mmTasks.SerializeAsString();
	/*send taskPkgStr*/
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

	MatrixMsg mmNumTask;
	mmNumTask.set_count(numTaskToSend);
	string strNumTask = mmNumTask.SerializeAsString();
	/* send back how many task will be sent*/

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

	wqMutex.lock();
	for (int i = 0; i < mm.count(); i++)
	{
		waitQueue.push_back(mm.tasks(i));

		/* update the task metadata in ZHT */
		string taskDetail;
		zc.lookup(mm.tasks(i).taskid(), taskDetail);

		Value value;
		value.ParseFromString(taskDetail);
		value.set_arrivetime(get_time_usec());
		value.set_nummove(value.nummove() + 1);
		value.set_history(value.history() + "->" + get_id());
		taskDetail = value.SerializeAsString();

		zc.insert(mm.tasks(i).taskid(), taskDetail);
		increment += 2;
	}
	wqMutex.unlock();

	string numTaskStr = num_to_str<long>(mm.count());
	/* send number of task string back*/
	if (increment > 0)
	{
		ZHTMsgCountMutex.lock();
		incre_ZHT_msg_count(increment);
		ZHTMsgCountMutex.unlock();
	}
}

void MatrixScheduler::recv_pushing_task(MatrixMsg &mm, int sockfd, sockaddr fromAddr)
{
	long increment = 0;

	lqMutex.lock();
	localQueue.push(mm.tasks(0));

	string taskDetail;
	zc.lookup(mm.tasks(0).taskid(), taskDetail);

	Value value;
	value.ParseFromString(taskDetail);
	value.set_rqueuedtime(get_time_usec());
	value.set_nummove(value.nummove() + 1);
	value.set_history(value.history() + "->" + get_id());
	taskDetail = value.SerializeAsString();

	zc.insert(mm.tasks(0).taskid(), taskDetail);
	increment += 2;

	/* send success */
	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();
}

/* processing requests received by the epoll server */
int MatrixScheduler::proc_req(int sockfd, void *buf, sockaddr fromAddr)
{
	MatrixMsg mm;
	string bufStr(buf);
	mm.ParseFromString(bufStr);

	long increment = 0;

	/* message type is stored in pkg.virtualpath(), and contents
	 * are stored in pkg.readfullpath() */
	string msg = mm.msgtype();
	if (msg.compare("query_load") == 0)	// thief quering load
	{
		int load = wsQueue.size();
		MatrixMsg mmLoad;
		mmLoad.set_msgtype("send load");
		mmLoad.set_count(load);
		string strLoad;
		strLoad = mmLoad.SerializeAsString();
		/* do a send back the load */
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
	long portNum = config->schedulerPortNo;
	string portStr = num_to_str<long>(portNum);
	const char *port = portStr.c_str();

	MatrixEpollServer *mes = new MatrixEpollServer(port, this);

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
//		send
//		recv(scheduler_vecotr.at[neighIdx[i]], config->scheduler_port_num, result, ***);
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

	long increment = 0;

	for (long i = 0; i < numRecv; i++)
	{
		string taskPkgStr;
		//recv(sockfd, taskPkgStr);
		MatrixMsg mm;
		mm.ParseFromString(taskPkgStr);

		wsqMutex.lock();
		for (long j = 0; j < mm.count(); j++)
		{
			wsQueue.push(mm.tasks(i));
			/* update task metadata */
			string taskDetailStr;
			zc.lookup(mm.tasks(i).taskid(), taskDetailStr);
			Value value;
			value.ParseFromString(taskDetailStr);
			value.set_nummove(value.nummove() + 1);
			value.set_history(value.history() + "->" + get_id());
			value.set_rqueuedtime(get_time_usec());
			taskDetailStr = value.SerializeAsString();
			zc.insert(mm.tasks(i).taskid(), taskDetailStr);
			increment += 2;
		}
		wsqMutex.unlock();
	}

	if (increment > 0)
	{
		ZHTMsgCountMutex.lock();
		incre_ZHT_msg_count(increment);
		ZHTMsgCountMutex.unlock();
	}
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
	int sockfd = 0;//send
	// recv(scheduler_vector.at(maxLoadedIdx), config->scheduler_port_num, taskStr, ***);
	MatrixMsg mmNumTask;
	mmNumTask.ParseFromString(numTaskPkgStr);

	int numTask = mmNumTask.count();

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
	while (ms->running)
	{
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
	}
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
void MatrixScheduler::exec_a_task(MatrixMsg_TaskMsg &tm)
{
	string taskDetail;
	zc.lookup(tm.taskid(), taskDetail);
	Value value;
	value.ParseFromString(taskDetail);
	value.set_exetime(get_time_usec());

	string data("");

	for (int i = 0; i < value.parents_size(); i++)
	{
		if (value.datasize_size() > 0)
		{
			if (value.parents(i).compare(get_id()) == 0)
			{
				data += localData.find(value.datanamelist(i))->second;
			}
			else
			{
				/* send and receive data for that task */;
			}
		}
	}

	const char *execmd = tm.cmd().c_str();
	string result = exec(execmd);
	string key = get_id() + tm.taskid() + "data";

	ldMutex.lock();
	localData.insert(make_pair(key, result));
	ldMutex.unlock();

	value.set_fintime(get_time_usec());
	taskDetail = value.SerializeAsString();
	zc.insert(tm.taskid(), taskDetail);

	numTaskFinMutex.lock();
	numTaskFin++;
	numTaskFinMutex.unlock();

	cqMutex.lock();
	completeQueue.push_back(CmpQueueItem(tm.taskid(), key, result.length()));
	cqMutex.unlock();

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(2);
	ZHTMsgCountMutex.unlock();
}

/* executing task thread function, under the conditin that the
 * scheduler is still processing tasks, as long as there are
 * tasks in the ready queue, execute the task one by one
 * */
void *executing_task(void *args)
{
	MatrixScheduler *ms = (MatrixScheduler*)args;
	MatrixMsg_TaskMsg tm;

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

			ms->numIdleCoreMutex.lock();
			ms->numIdleCore--;
			ms->numIdleCoreMutex.unlock();

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
		while (pthread_create(&execThread[i], NULL, executing_task, this))
		{
			sleep(1);
		}
	}
}

bool MatrixScheduler::task_ready_process(
		const Value &valuePkg, MatrixMsg_TaskMsg &tm)
{
	bool flag = false;

	if (valuePkg.alldatasize() <= config->dataSizeThreshold)
	{
		tm.set_datalength(valuePkg.alldatasize());
		wsqMutex.lock();
		wsQueue.push(tm);
		wsqMutex.unlock();
		flag = true;
	}
	else
	{
		long maxDataSize = -1000000;
		string maxDataScheduler;
		for (int i = 0; i < valuePkg.datasize_size(); i++)
		{
			if (valuePkg.datasize(i) > maxDataSize)
			{
				maxDataSize = valuePkg.datasize(i);
				maxDataScheduler = valuePkg.parents(i);
			}
		}
		tm.set_datalength(maxDataSize);
		if (maxDataScheduler.compare(get_id()) == 0)
		{
			lqMutex.lock();
			localQueue.push(tm);
			lqMutex.unlock();
			flag = true;
		}
		else
		{
			MatrixMsg mm;
			mm.set_msgtype("scheduler push task");
			mm.set_count(1);
			MatrixMsg_TaskMsg *mmtm = mm.add_tasks();
			*mmtm = tm;
			string mmStr = mm.SerializeAsString();
			/* send a message to maxDataScheduler, and recv ack*/
		}
	}

	return flag;
}
/* check to see whether a task is ready to run or not. A task is
 * ready only if all of its parants are done (the indegree counter
 * equals to 0).
 * */
long MatrixScheduler::check_a_ready_task(MatrixMsg_TaskMsg &tm)
{
	string taskDetail;
	long incre = 0;

	zc.lookup(tm.taskid(), taskDetail);
	incre++;

	Value valuePkg;
	valuePkg.ParseFromString(taskDetail);

	if (valuePkg.indegree() == 0)
	{
		if (task_ready_process(valuePkg, tm))
		{
			valuePkg.set_rqueuedtime(get_time_usec());
			taskDetail = valuePkg.SerializeAsString();
			zc.insert(tm.taskid(), taskDetail);
			incre++;
		}
		return incre;
	}

	return 0;
}

bool check_empty(MatrixMsg_TaskMsg &tm)
{
	return tm.taskid().empty();
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
	int size = 0;
	MatrixMsg_TaskMsg tm;
	long increment = 0;

	while (ms->running)
	{
		while (ms->waitQueue.size() > 0)
		{
			size = ms->waitQueue.size();

			for (int i = 0; i < size; i++)
			{
				tm = ms->waitQueue[i];
				if (!tm.taskid().empty())
				{
					int ret = ms->check_a_ready_task(tm);
					if (ret != 0)
					{
						increment += ret;
						ms->waitQueue[i].set_taskid("");
					}
					else
					{
						increment += 1;
					}
				}
			}

			/* erase all the task entries that have been
			 * moved to the ready queue. Those entries are
			 * set to be empty
			 * */
			ms->wqMutex.lock();
			deque<MatrixMsg_TaskMsg>::iterator last = remove_if(ms->waitQueue.begin(),
					ms->waitQueue.end(), check_empty);
			ms->waitQueue.erase(last, ms->waitQueue.end());
			ms->wqMutex.unlock();
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

	zc.lookup(cqItem.taskId, taskDetail);
	Value value;
	value.ParseFromString(taskDetail);

	increment++;
	string childTaskId, childTaskDetail, childTaskDetailAttempt, query_value;
	Value childVal;

	for (int i = 0; i < value.children_size(); i++)
	{
		childTaskId = value.children(i);
		zc.lookup(childTaskId, childTaskDetail);
		increment++;
		childVal.ParseFromString(childTaskDetail);
		childVal.set_indegree(childVal.indegree() - 1);
		childVal.add_parents(get_id());
		childVal.add_datanamelist(cqItem.key);
		childVal.add_datasize(cqItem.dataSize);
		childTaskDetailAttempt = childVal.SerializeAsString();

		increment++;
		while (zc.compare_swap(childTaskId, childTaskDetail, childTaskDetailAttempt, query_value) != 0)
		{
			childTaskDetail = query_value;
			childVal.ParseFromString(childTaskDetail);
			childVal.set_indegree(childVal.indegree() - 1);
			childVal.add_parents(get_id());
			childVal.add_datanamelist(cqItem.key);
			childVal.add_datasize(cqItem.dataSize);
			childTaskDetailAttempt = childVal.SerializeAsString();
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
	CmpQueueItem cqItem;

	long increment = 0;

	while (ms->running)
	{
		while (ms->completeQueue.size() > 0)
		{
			ms->cqMutex.lock();
			if (ms->completeQueue.size() > 0)
			{
				cqItem = ms->completeQueue.front();
				ms->completeQueue.pop_back();
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
		string recordValStr = recordVal.SerializeAsString();
		ms->zc.insert(ms->get_id(), recordValStr);

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
		ms->zc.lookup(key, numTaskDoneStr);

		increment += 2;

		long numTaskDone = str_to_num<long>(numTaskDoneStr);
		if (numTaskDone == ms->config->numAllTask)
		{
			ms->running = false;
			if (ms->schedulerLogOS.is_open())
			{
				ms->schedulerLogOS << get_time_usec() << "\t" << ms->numTaskFin << "\t" <<
						ms->waitQueue.size() << "\t" << ms->localQueue.size() + ms->wsQueue.size()
						<< "\t" << ms->numIdleCore << "\t" << ms->config->numCorePerExecutor <<
						"\t" << ms->numWS << "\t" << ms->numWSFail << endl;
				ms->schedulerLogOS << "The number of ZHT message is:" << ms->numZHTMsg << endl;
				ms->schedulerLogOS.flush(); ms->schedulerLogOS.close();
			}
			break;
		}

		ms->numTaskFinMutex.lock();

		numTaskDone += (ms->numTaskFin - ms->prevNumTaskFin);
		string numTaskDoneStrNew = num_to_str<long>(numTaskDone);
		string query_value;
		increment++;
		while (ms->zc.compare_swap(key, numTaskDoneStr,
				numTaskDoneStrNew, query_value) != 0)
		{
			numTaskDoneStr = query_value;
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

	pthread_exit(NULL);
	return NULL;
}

/* fork recording status thread */
void MatrixScheduler::fork_record_stat_thread()
{
	pthread_t rsThread;

	while (pthread_create(&rsThread, NULL, recording_stat, this))
	{
		sleep(1);
	}
}

CmpQueueItem::CmpQueueItem(const string &taskId,
		const string &key, long dataSize)
{
	this->taskId = taskId;
	this->key = key;
	this->dataSize;
}
