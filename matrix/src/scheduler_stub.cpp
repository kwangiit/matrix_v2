/*
 * scheduler_stub.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"
#include "math.h"
#include <algorithm>

MatrixScheduler::MatrixScheduler(const string
		&configFile):Peer(configFile)
{
	timespec start, end;
	clock_gettime(0, &start);

	numNeigh = (int)(sqrt(schedulerVec.size()));
	neighIdx = new int[numNeigh];
	maxLoadedIdx = -1;
	maxLoad = -1000000;
	pollInterval = config->wsPollIntervalStart;
	chooseBitMap = new bool[schedulerVec.size()];
	reset_choosebm();

	ZHTMsgCountMutex = new Mutex();
	numIdleCoreMutex = new Mutex();
	numTaskFinMutex = new Mutex();

	wqMutex = new Mutex();
	rqMutex = new Mutex();
	cqMutex = new Mutex();

	clock_gettime(0, &end);

	if (config->schedulerLog.compare("yes") == 0)
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

void MatrixScheduler::regist(ZHTClient &zc)
{
	string regKey("number of scheduler registered");
	string taskFinKey("num tasks done");
	long increment = 0;
	if (index == 0)
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
			usleep(1);
		}
	}
	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();
}

void MatrixScheduler::pack_send_task(int numTask, int sockfd, sockaddr fromAddr)
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

void MatrixScheduler::send_task(int sockfd, sockaddr fromAddr)
{
	int numTaskToSend = -1;
	rqMutex.lock();
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

int MatrixScheduler::proc_req(int sockfd, void *buf, sockaddr fromAddr)
{
	Package pkg;
	pkg.ParseFromArray(buf, _BUF_SIZE);

	string msg = pkg.virtualpath();
	if (msg.compare("query_load") == 0)
	{
		int load = readyQueue.size() - numIdleCore;
		/* do a send back the load */
	}
	else if (msg.compare("steal task") == 0)
	{
		send_task(sockfd, fromAddr);
	}
	else if (msg.compare("send task") == 0)
	{
		/* add tasks and then send ack back */
		string taskStr = pkg.realfullpath();
		vector<string> tasks = tokenize(taskStr, "eot");
		wqMutex.lock();
		for (int i = 0; i < tasks.size(); i++)
		{
			waitQueue.push_back(tasks.at(i));
		}
		wqMutex.unlock();
		string numTaskStr = num_to_str<int>(tasks.size());
		/* send number of task string back*/
	}

	return 1;
}

void* MatrixScheduler::epoll_serving(void *args)
{
	MatrixEpollServer *mes = (MatrixEpollServer*)args;
	mes->serve();
	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_es_thread()
{
	int portNum = config->schedulerPortNo;
	string portStr = num_to_str<int>(portNum);
	char *port = portStr.c_str();
	MatrixEpollServer mes = new MatrixEpollServer(port, this);
	pthread_t esThread;
	while (pthread_create(&esThread, NULL, epoll_serving, &mes) != 0)
	{
		sleep(1);
	}
}

void MatrixScheduler::reset_choosebm()
{
	for (int i = 0; i < schedulerVec.size(); i++)
	{
		chooseBitMap[i] = false;
	}
}

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

void MatrixScheduler::find_most_loaded_neigh()
{
	Package loadQueryPkg;
	loadQueryPkg.set_virtualpath("query load");
	string strLoadQuery = loadQueryPkg.SerializeAsString();

	int load = -1;
	for (int i = 0; i < numNeigh; i++)
	{
		string result;
//		send
//		recv(scheduler_vecotr.at[neighIdx[i]], config->scheduler_port_num, result, ***);
		Package loadPkg;
		loadPkg.ParseFromString(result);
		string loadStr = loadPkg.realfullpath();
		load = str_to_num<int>(loadStr);
		if (maxLoad < load)
		{
			maxLoad = load;
			maxLoadedIdx = neighIdx[i];
		}
	}
}

bool MatrixScheduler::steal_task()
{
	if (maxLoad <= 0)
	{
		return false;
	}

	Package stealTaskPkg;
	stealTaskPkg.set_virtualpath("steal task");
	string strStealTask = stealTaskPkg.SerializeAsString();

	string numTaskPkgStr;
	// send
	// recv(scheduler_vector.at(maxLoadedIdx), config->scheduler_port_num, taskStr, ***);
	Package numTaskPkg;
	numTaskPkg.ParseFromString(numTaskPkgStr);
	string numTaskStr = numTaskPkg.realfullpath();
	int numTask = str_to_num<int>(numTaskStr);

	if (numTask == 0)
	{
		return false;
	}

	int numRecv = numTask / config->maxTaskPerPkg;
	if (numRecv * config->maxTaskPerPkg < numTask)
	{
		numRecv++;
	}

	for (int i = 0; i < numRecv; i++)
	{
		string taskPkgStr;
		//recv()
		Package taskPkg;
		taskPkg.ParseFromString(taskPkgStr);
		vector<string> taskStrVec = tokenize(taskPkg.realfullpath(), "eot");
		rqMutex.lock();
		for (int j = 0; j < taskStrVec.size(); j++)
		{
			readyQueue.push_back(taskStrVec.at(j));
		}
		rqMutex.unlock();
	}

	return true;
}

void* MatrixScheduler::workstealing(void*)
{
	while (running)
	{
		while (readyQueue.size() == 0 && pollInterval < config->wsPollIntervalUb)
		{
			choose_neigh();
			find_most_loaded_neigh();
			bool success = steal_task();
			numWS++;
			maxLoadedIdx = -1;
			maxLoad = -1000000;
			if (success)
			{
				pollInterval = config->wsPollIntervalStart;
			}
			else
			{
				numWSFail++;
				usleep(pollInterval);
				pollInterval *= 2;
			}
		}

		if (pollInterval >= config->wsPollIntervalUb)
		{
			break;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_ws_thread()
{
	if (config->workStealingOn == 1)
	{
		pthread_t wsThread;
		while (pthread_create(&wsThread, NULL, workstealing, NULL))
		{
			sleep(1);
		}
	}
}

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

	char *execmd = (taskStrVec.at(3) + taskStrVec.at(4)).c_str();
	string result = exec(execmd);
	numTaskFinMutex.lock();
	numTaskFin++;
	numTaskFinMutex.unlock();
	cqMutex.lock();
	completeQueue.push_back(taskStrVec.at(0));
	cqMutex.unlock();
}

void* MatrixScheduler::executing_task(void*)
{
	string taskStr;

	while (running)
	{
		while (readyQueue.size() > 0)
		{
			rqMutex.lock();
			if (readyQueue.size > 0)
			{
				taskStr = readyQueue.front();
				readyQueue.pop_front();
				rqMutex.unlock();
			}
			else
			{
				rqMutex.unlock();
				continue;
			}
			numIdleCoreMutex.lock();
			numIdleCore--;
			numIdleCoreMutex.unlock();

			exec_a_task(taskStr);

			numIdleCoreMutex.lock();
			numIdleCore++;
			numIdleCoreMutex.unlock();
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_exec_task_thread()
{
	pthread_t *execThread = new pthread_t[config->numCorePerExecutor];

	for (int i = 0; i < config->numCorePerExecutor; i++)
	{
		while (pthread_create(&execThread[i], NULL, executing_task, NULL))
		{
			sleep(1);
		}
	}
}

bool MatrixScheduler::check_a_ready_task(const string &taskStr, ZHTClient *zc)
{
	vector<string> taskStrVec = tokenize(taskStr, " ");
	string taskDetail;
	zc->lookup(taskStrVec.at(0), taskDetail);

	Value valuePkg;
	valuePkg.ParseFromString(taskDetail);
	if (valuePkg.indegree() == 0)
	{
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

void* MatrixScheduler::checking_ready_task(void *args)
{
	ZHTClient *zc = (ZHTClient*)args;
	int size = 0;
	string taskStr;
	long increment = 0;

	while (running)
	{
		while (waitQueue.size() > 0)
		{
			size = waitQueue.size();
			for (int i = 0; i < size; i++)
			{
				taskStr = waitQueue[i];
				if (!taskStr.empty())
				{
					increment++;
					if (check_a_ready_task(taskStr, zc))
					{
						rqMutex.lock();
						readyQueue.push_back(taskStr);
						//vector<string> taskStrVec = tokenize(taskStr, " ");
						rqMutex.unlock();
						wqMutex.lock();
						waitQueue[i] = "";
						wqMutex.unlock();
					}
				}
			}
			wqMutex.lock();
			deque::iterator last = remove_if(waitQueue.begin(),
					waitQueue.end(), check_empty);
			waitQueue.erase(last, waitQueue.end());
			wqMutex.unlock();
		}
	}

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_crt_thread(ZHTClient &zc)
{
	pthread_t crtThread;


	while (pthread_create(&crtThread, NULL, checking_ready_task, &zc))
	{
		sleep(1);
	}
}

long MatrixScheduler::decrease_indegree(const string &taskId, ZHTClient *zc)
{
	string taskDetail;
	long increment = 0;

	zc->lookup(taskId, taskDetail);
	increment++;

	Value taskDetailVal;
	taskDetailVal.ParseFromString(taskDetail);

	int numChildren = taskDetailVal.children_size();
	string childTaskId, childTaskDetail;
	for (int i = 0; i < numChildren; i++)
	{
		childTaskId = taskDetailVal.children(i);
		zc->lookup(childTaskId, childTaskDetail);
		Value childTaskDetailVal;
		childTaskDetailVal.ParseFromString(childTaskDetail);
		childTaskDetailVal.set_indegree(childTaskDetailVal.indegree() - 1);
		childTaskDetail = childTaskDetailVal.SerializeAsString();
		zc->insert(childTaskId, childTaskDetail);
		increment += 2;
	}

	return increment;
}

void* MatrixScheduler::checking_complete_task(void *args)
{
	ZHTClient *zc = (ZHTClient*)args;
	string taskId;
	long increment = 0;

	while (running)
	{
		while (completeQueue.size() > 0)
		{
			cqMutex.lock();
			if (completeQueue.size() > 0)
			{
				taskId = completeQueue.front();
				completeQueue.pop_back();
				cqMutex.unlock();
			}
			else
			{
				cqMutex.unlock();
				continue;
			}
			increment += decrease_indegree(taskId, zc);
		}
	}

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_cct_thread(ZHTClient &zc)
{
	pthread_t cctThread;

	while (pthread_create(&cctThread, NULL, checking_complete_task, &zc))
	{
		sleep(1);
	}
}

void* MatrixScheduler::recording_stat(void *args)
{
	ZHTClient *zc = (ZHTClient*)args;
	long increment = 0;

	timespec time;
	bool schedulerLogOn = false;
	if (schedulerLogOS.is_open())
	{
		schedulerLogOn = true;
		schedulerLogOS << "Time\tNumTaskFin\tNumTaskWait\tNumTaskReady\t"
				"NumIdleCore\tNumAllCore\tNumWorkSteal\tNumWorkStealFail" << endl;
	}

	while (1)
	{
		Value recordVal;
		recordVal.set_id(get_id());
		recordVal.set_numtaskfin(numTaskFin);
		recordVal.set_numtaskwait(waitQueue.size());
		recordVal.set_numtaskready(readyQueue.size());
		recordVal.set_numcoreavilable(numIdleCore);
		recordVal.set_numallcore(config->numCorePerExecutor);
		recordVal.set_numworksteal(numWS);
		recordVal.set_numworkstealfail(numWSFail);
		string recordValStr = recordVal.SerializeAsString();
		zc->insert(get_id(), recordValStr);

		if (schedulerLogOn)
		{
			schedulerLogOS << get_time_usec() << "\t" << numTaskFin << "\t" <<
					waitQueue.size() << "\t" << readyQueue.size() << "\t" <<
					numIdleCore << "\t" << config->numCorePerExecutor << "\t" <<
					numWS << "\t" << numWSFail << endl;
		}

		string key("num tasks done");
		string numTaskDoneStr;
		zc->lookup(key, numTaskDoneStr);
		increment += 2;
		long numTaskDone = str_to_num<long>(numTaskDoneStr);
		if (numTaskDone == config->numAllTask)
		{
			running = false;
			if (schedulerLogOn)
			{
				schedulerLogOS << get_time_usec() << "\t" << numTaskFin << "\t" <<
						waitQueue.size() << "\t" << readyQueue.size() << "\t" <<
						numIdleCore << "\t" << config->numCorePerExecutor << "\t" <<
						numWS << "\t" << numWSFail << endl;
				schedulerLogOS << "The number of ZHT message is:" << numZHTMsg << endl;
				schedulerLogOS.flush(); schedulerLogOS.close();
			}
			break;
		}
		numTaskFinMutex.lock();
		numTaskDone += (numTaskFin - prevNumTaskFin);
		stringstream ss;
		ss << numTaskDone;
		string numTaskDoneStrNew = ss.str();
		string query_value;
		increment++;
		while (zc->compare_swap(key, numTaskDoneStr,
				numTaskDoneStrNew, query_value) != 0)
		{
			numTaskDoneStr = query_value;
			numTaskDone = str_to_num<long>(numTaskDoneStr);
			if (numTaskDone == config->numAllTask)
			{
				break;
			}
			numTaskDone += (numTaskFin - prevNumTaskFin);
			ss.str("");
			ss << numTaskDone;
			numTaskDoneStrNew = ss.str();
			increment++;
		}
		prevNumTaskFin = numTaskFin;

		numTaskFinMutex.unlock();
		usleep(config->sleepLength);
	}

	ZHTMsgCountMutex.lock();
	incre_ZHT_msg_count(increment);
	ZHTMsgCountMutex.unlock();

	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::fork_record_stat_thread(ZHTClient &zc)
{
	pthread_t rsThread;

	while (pthread_create(&rsThread, NULL, recording_stat, &zc))
	{
		sleep(1);
	}
}
