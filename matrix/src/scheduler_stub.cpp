/*
 * scheduler_stub.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"
#include "math.h"
#include <algorithm>

MatrixScheduler::MatrixScheduler(const string &config_file)
{
	config = new Configuration(config_file);
	setHostName(getHostIdentity(config->host_identity_type));
	scheduler_vector = readFromFile(config->scheduler_memList_file);
	setIndex(getSelfIndex(getHostName(), scheduler_vector));

	numNeigh = (int)(sqrt(scheduler_vector.size()));
	neighIdx = new int[numNeigh];
	maxLoadedIdx = -1;
	maxLoad = -1000000;
	pollInterval = config->ws_poll_interval_start;
	chooseBitMap = new bool[scheduler_vector.size()];
	resetChooseBM();

	numIdleCoreMutex = new Mutex();
	numTaskFinMutex = new Mutex();

	wqMutex = new Mutex();
	rqMutex = new Mutex();
	cqMutex = new Mutex();
}

void MatrixScheduler::regist(ZHTClient &zc)
{
	string key("number of scheduler registered");
	if (index == 1)
	{
		zc.insert(key, "1");
	}
	else
	{
		string value;
		zc.lookup(key, value);
		while (value.empty())
		{
			usleep(config->sleep_lengh);
			zc.lookup(key, value);
		}
		int newValNum = getInt(value) + 1;
		stringstream ss;
		ss << newValNum;
		string newVal(ss.str());
		string queryVal;
		while (zc.compare_swap(key, value, newVal, queryVal) != 0)
		{
			ss.str("");
			value = queryVal;
			newValNum = getInt(value) + 1;
			ss << newValNum;
			newVal = ss.str();
		}
	}
}

void MatrixScheduler::resetChooseBM()
{
	for (int i = 0; i < scheduler_vector.size(); i++)
	{
		chooseBitMap[i] = false;
	}
}

void MatrixScheduler::chooseNeigh()
{
	srand(time(NULL));
	int idx = -1;
	for (int i = 0; i < numNeigh; i++)
	{
		idx = rand() % scheduler_vector.size();
		while (idx == getIndex() || chooseBitMap[idx])
		{
			idx = rand() % scheduler_vector.size();
		}
		neighIdx[i] = idx;
		chooseBitMap[idx] = true;
	}
	resetChooseBM();
}

void MatrixScheduler::findMostLoadedNeigh()
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
		load = getInt(loadPkg.realfullpath());
		if (maxLoad < load)
		{
			maxLoad = load;
			maxLoadedIdx = neighIdx[i];
		}
	}
}

bool MatrixScheduler::stealTask()
{
	if (maxLoad <= 0)
	{
		return false;
	}

	Package stealTaskPkg;
	stealTaskPkg.set_virtualpath("steal task");
	string strStealTask = stealTaskPkg.SerializeAsString();

	string taskStr;
	// send
	// recv(scheduler_vector.at(maxLoadedIdx), config->scheduler_port_num, taskStr, ***);
	Package taskPkg;
	taskPkg.ParseFromString(taskStr);

	if (taskPkg.realfullpath().empty())
	{
		return false;
	}
	else
	{
		vector<string> taskStrVec = tokenize(taskPkg.realfullpath(), "eot");
		rqMutex.lock();
		for (int i = 0; i < taskStrVec.size(); i++)
		{
			readyQueue.push_back(taskStrVec.at(i));
		}
		rqMutex.unlock();
		return true;
	}
}

void* MatrixScheduler::workstealing(void*)
{
	while (1)
	{
		 while (readyQueue.size() == 0 && pollInterval < config->ws_poll_interval_ub)
		 {
			 chooseNeigh();
			 findMostLoadedNeigh();
			 bool success = stealTask();
			 if (success)
			 {
				 pollInterval = config->ws_poll_interval_start;
			 }
			 else
			 {
				 usleep(pollInterval);
				 pollInterval *= 2;
			 }
		 }

		 if (pollInterval >= config->ws_poll_interval_ub)
		 {
			 break;
		 }
	}
	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::forkWSThread()
{
	if (config->work_stealing_on == 1)
	{
		pthread_t wsThread;
		while (pthread_create(&wsThread, NULL, workstealing, NULL))
		{
			sleep(1);
		}
	}
}

void MatrixScheduler::execOneTask(string &taskStr)
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

void* MatrixScheduler::executingTask(void*)
{
	string taskStr;

	while (1)
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

			execOneTask(taskStr);

			numIdleCoreMutex.lock();
			numIdleCore++;
			numIdleCoreMutex.unlock();
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::forkETThread()
{
	pthread_t *execThread = new pthread_t[config->num_core_per_executor];

	for (int i = 0; i < config->num_core_per_executor; i++)
	{
		while (pthread_create(&execThread[i], NULL, executingTask, NULL))
		{
			sleep(1);
		}
	}
}

bool MatrixScheduler::checkAReadyTask(const string &taskStr, ZHTClient *zc)
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

bool checkEmpty(string &str)
{
	return str.empty();
}

void* MatrixScheduler::checkingReadyTask(void *args)
{
	ZHTClient *zc = (ZHTClient*)args;
	int size = 0;
	string taskStr;
	while (1)
	{
		while (waitQueue.size() > 0)
		{
			size = waitQueue.size();
			for (int i = 0; i < size; i++)
			{
				taskStr = waitQueue[i];
				if (!taskStr.empty())
				{
					if (checkAReadyTask(taskStr, zc))
					{
						rqMutex.lock();
						readyQueue.push_back(taskStr);
						rqMutex.unlock();
						wqMutex.lock();
						waitQueue[i] = "";
						wqMutex.unlock();
					}
				}
			}
			wqMutex.lock();
			deque::iterator last = remove_if(waitQueue.begin(),
										waitQueue.end(), checkEmpty);
			waitQueue.erase(last, waitQueue.end());
			wqMutex.unlock();
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void MatrixScheduler::forkCRTThread()
{
	pthread_t crtThread;

	while (pthread_create(&crtThread, NULL, checkingReadyTask, NULL))
	{
		sleep(1);
	}
}



void MatrixScheduler::setHostName(string hostname)
{
	this->hostname = hostname;
}

string MatrixScheduler::getHostName()
{
	return hostname;
}

void MatrixScheduler::setIndex(int index)
{
	this->index = index;
}

int MatrixScheduler::getIndex()
{
	return index;
}
