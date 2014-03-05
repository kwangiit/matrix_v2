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
	string tasks;

	for (int j = 0; j < numTask; j++)
	{
		tasks += readyQueue.front();
		tasks += "eot";
		readyQueue.pop_front();
	}

	Package taskPkg;
	taskPkg.set_realfullpath(tasks);
	string taskPkgStr = taskPkg.SerializeAsString();
	/*send taskPkgStr*/
}

/* send tasks to another thief scheduler */
void MatrixScheduler::send_task(int sockfd, sockaddr fromAddr)
{
	int numTaskToSend = -1;

	rqMutex.lock();

	/* number of tasks to send equals to half of the current load,
	 * which is calculated as the number of tasks in the ready queue
	 * minus number of idle cores */
	numTaskToSend = (readyQueue.size() - numIdleCore) / 2;

	Package numTaskPkg;
	numTaskPkg.set_realfullpath(num_to_str<int>(numTaskToSend));
	string numTaskPkgStr = numTaskPkg.SerializeAsString();
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
	rqMutex.unlock();
}

/* receive tasks submitted by client */
void MatrixScheduler::recv_task_from_client(
		string &taskStr, int sockfd, sockaddr fromAddr)
{
	long increment = 0;

	/* tasks are delimited with "eot"*/
	vector<string> tasks = tokenize(taskStr, "eot");

	wqMutex.lock();
	for (int i = 0; i < tasks.size(); i++)
	{
		waitQueue.push_back(tasks.at(i));

		/* update the task metadata in ZHT */
		vector<string> taskVec = tokenize(tasks.at(i), " ");
		string taskDetail;
		zc.lookup(taskVec.at(0), taskDetail);

		Value value;
		value.ParseFromString(taskDetail);
		value.set_arrivetime(get_time_usec());
		value.set_nummove(value.nummove() + 1);
		value.set_history(value.history() + "->" +
				num_to_str<int>(get_index()));
		taskDetail = value.SerializeAsString();

		zc.insert(taskVec.at(0), taskDetail);
		increment += 2;
	}
	wqMutex.unlock();

	string numTaskStr = num_to_str<int>(tasks.size());
	/* send number of task string back*/
	if (increment > 0)
	{
		ZHTMsgCountMutex.lock();
		incre_ZHT_msg_count(increment);
		ZHTMsgCountMutex.unlock();
	}
}

/* processing requests received by the epoll server */
int MatrixScheduler::proc_req(int sockfd, void *buf, sockaddr fromAddr)
{
	Package pkg;
	pkg.ParseFromArray(buf, _BUF_SIZE);

	long increment = 0;

	/* message type is stored in pkg.virtualpath(), and contents
	 * are stored in pkg.readfullpath() */
	string msg = pkg.virtualpath();
	if (msg.compare("query_load") == 0)	// thief quering load
	{
		int load = readyQueue.size() - numIdleCore;
		/* do a send back the load */
	}
	else if (msg.compare("steal task") == 0)	// thief steals tasks
	{
		send_task(sockfd, fromAddr);
	}
	else if (msg.compare("send task") == 0)	// client sent tasks
	{
		/* add tasks and then send ack back */
		string taskStr = pkg.realfullpath();
		recv_task_from_client(taskStr, sockfd, fromAddr);
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
	Package loadQueryPkg;
	loadQueryPkg.set_virtualpath("query load");
	string strLoadQuery = loadQueryPkg.SerializeAsString();

	long load = -1;

	for (int i = 0; i < numNeigh; i++)
	{
		string result;
//		send
//		recv(scheduler_vecotr.at[neighIdx[i]], config->scheduler_port_num, result, ***);
		Package loadPkg;
		loadPkg.ParseFromString(result);
		string loadStr = loadPkg.realfullpath();
		load = str_to_num<long>(loadStr);

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
		Package taskPkg;
		taskPkg.ParseFromString(taskPkgStr);
		vector<string> taskStrVec = tokenize(taskPkg.realfullpath(), "eot");

		rqMutex.lock();
		for (long j = 0; j < taskStrVec.size(); j++)
		{
			readyQueue.push_back(taskStrVec.at(j));
			/* update task metadata */
			vector<string> taskSpec = tokenize(taskStrVec.at(j), " ");
			string taskDetailStr;
			zc.lookup(taskSpec.at(0), taskDetailStr);
			Value value;
			value.ParseFromString(taskDetailStr);
			value.set_nummove(value.nummove() + 1);
			value.set_history(value.history() + "->" + num_to_str<int>(get_index()));
			value.set_rqueuedtime(get_time_usec());
			taskDetailStr = value.SerializeAsString();
			zc.insert(taskSpec.at(0), taskDetailStr);
			increment += 2;
		}
		rqMutex.unlock();
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

	Package stealTaskPkg;
	stealTaskPkg.set_virtualpath("steal task");
	string strStealTask = stealTaskPkg.SerializeAsString();

	string numTaskPkgStr;
	int sockfd = 0;//send
	// recv(scheduler_vector.at(maxLoadedIdx), config->scheduler_port_num, taskStr, ***);
	Package numTaskPkg;
	numTaskPkg.ParseFromString(numTaskPkgStr);
	string numTaskStr = numTaskPkg.realfullpath();
	int numTask = str_to_num<int>(numTaskStr);

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
		while (ms->readyQueue.size() == 0 &&
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
void MatrixScheduler::exec_a_task(string &taskStr)
{
	/*
	 * taskStrVec.at(0) = taskId
	 * taskStrVec.at(1) = user
	 * taskStrVec.at(2) = directory
	 * taskStrVec.at(3) = cmd
	 * taskStrVec.at(4) = arguments
	 */
	vector<string> taskStrVec = tokenize(taskStr, " ");
	string taskDetail;
	zc.lookup(taskStrVec.at(0), taskDetail);
	Value value;
	value.ParseFromString(taskDetail);
	value.set_exetime(get_time_usec());

	const char *execmd = (taskStrVec.at(3) + taskStrVec.at(4)).c_str();
	string result = exec(execmd);
	value.set_fintime(get_time_usec());
	taskDetail = value.SerializeAsString();
	zc.insert(taskStrVec.at(0), taskDetail);

	numTaskFinMutex.lock();
	numTaskFin++;
	numTaskFinMutex.unlock();
	cqMutex.lock();
	completeQueue.push_back(taskStrVec.at(0));
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
	string taskStr;

	while (ms->running)
	{
		while (ms->readyQueue.size() > 0)
		{
			ms->rqMutex.lock();
			if (ms->readyQueue.size() > 0)
			{
				taskStr = ms->readyQueue.front();
				ms->readyQueue.pop_front();
				ms->rqMutex.unlock();
			}
			else
			{
				ms->rqMutex.unlock();
				continue;
			}
			ms->numIdleCoreMutex.lock();
			ms->numIdleCore--;
			ms->numIdleCoreMutex.unlock();

			ms->exec_a_task(taskStr);

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

/* check to see whether a task is ready to run or not. A task is
 * ready only if all of its parants are done (the indegree counter
 * equals to 0).
 * */
bool MatrixScheduler::check_a_ready_task(const string &taskStr)
{
	vector<string> taskStrVec = tokenize(taskStr, " ");
	string taskDetail;
	zc.lookup(taskStrVec.at(0), taskDetail);

	Value valuePkg;
	valuePkg.ParseFromString(taskDetail);
	if (valuePkg.indegree() == 0)
	{
		valuePkg.set_rqueuedtime(get_time_usec());
		taskDetail = valuePkg.SerializeAsString();
		zc.insert(taskStrVec.at(0), taskDetail);
		return true;
	}
	else
	{
		return false;
	}
}

bool check_empty(string &str)
{
	return str.empty();
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
	string taskStr;
	long increment = 0;

	while (ms->running)
	{
		while (ms->waitQueue.size() > 0)
		{
			size = ms->waitQueue.size();

			for (int i = 0; i < size; i++)
			{
				taskStr = ms->waitQueue[i];
				if (!taskStr.empty())
				{
					increment++;
					if (ms->check_a_ready_task(taskStr))
					{
						increment++;
						ms->rqMutex.lock();
						ms->readyQueue.push_back(taskStr);
						ms->rqMutex.unlock();

						ms->wqMutex.lock();
						ms->waitQueue[i] = "";
						ms->wqMutex.unlock();
					}
				}
			}

			/* erase all the task entries that have been
			 * moved to the ready queue. Those entries are
			 * set to be empty
			 * */
			ms->wqMutex.lock();
			deque<string>::iterator last = remove_if(ms->waitQueue.begin(),
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
long MatrixScheduler::decrease_indegree(const string &taskId)
{
	string taskDetail;
	long increment = 0;

	zc.lookup(taskId, taskDetail);
	increment++;

	Value taskDetailVal;
	taskDetailVal.ParseFromString(taskDetail);

	int numChildren = taskDetailVal.children_size();
	string childTaskId, childTaskDetail;
	for (int i = 0; i < numChildren; i++)
	{
		childTaskId = taskDetailVal.children(i);
		zc.lookup(childTaskId, childTaskDetail);
		Value childTaskDetailVal;
		childTaskDetailVal.ParseFromString(childTaskDetail);
		childTaskDetailVal.set_indegree(childTaskDetailVal.indegree() - 1);
		childTaskDetail = childTaskDetailVal.SerializeAsString();
		zc.insert(childTaskId, childTaskDetail);
		increment += 2;
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
	string taskId;
	long increment = 0;

	while (ms->running)
	{
		while (ms->completeQueue.size() > 0)
		{
			ms->cqMutex.lock();
			if (ms->completeQueue.size() > 0)
			{
				taskId = ms->completeQueue.front();
				ms->completeQueue.pop_back();
				ms->cqMutex.unlock();
			}
			else
			{
				ms->cqMutex.unlock();
				continue;
			}
			increment += ms->decrease_indegree(taskId);
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
	bool schedulerLogOn = false;
	if (ms->schedulerLogOS.is_open())
	{
		schedulerLogOn = true;
		ms->schedulerLogOS << "Time\tNumTaskFin\tNumTaskWait\tNumTaskReady\t"
				"NumIdleCore\tNumAllCore\tNumWorkSteal\tNumWorkStealFail" << endl;
	}

	while (1)
	{
		Value recordVal;
		recordVal.set_id(ms->get_id());
		recordVal.set_numtaskfin(ms->numTaskFin);
		recordVal.set_numtaskwait(ms->waitQueue.size());
		recordVal.set_numtaskready(ms->readyQueue.size());
		recordVal.set_numcoreavilable(ms->numIdleCore);
		recordVal.set_numallcore(ms->config->numCorePerExecutor);
		recordVal.set_numworksteal(ms->numWS);
		recordVal.set_numworkstealfail(ms->numWSFail);
		string recordValStr = recordVal.SerializeAsString();
		ms->zc.insert(ms->get_id(), recordValStr);

		if (schedulerLogOn)
		{
			ms->schedulerLogOS << get_time_usec() << "\t" << ms->numTaskFin << "\t" <<
					ms->waitQueue.size() << "\t" << ms->readyQueue.size() << "\t" <<
					ms->numIdleCore << "\t" << ms->config->numCorePerExecutor << "\t" <<
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
			if (schedulerLogOn)
			{
				ms->schedulerLogOS << get_time_usec() << "\t" << ms->numTaskFin << "\t" <<
						ms->waitQueue.size() << "\t" << ms->readyQueue.size() << "\t" <<
						ms->numIdleCore << "\t" << ms->config->numCorePerExecutor << "\t" <<
						ms->numWS << "\t" << ms->numWSFail << endl;
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
