/*
 * scheduler_stub.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"
#include "math.h"

MatrixScheduler::MatrixScheduler(const string &config_file)
{
	config = new Configuration(config_file);
	setHostName(getHostIdentity(config->host_identity_type));
	scheduler_vector = readFromFile(config->scheduler_memList_file);
	setIndex(getSelfIndex(getHostName(), scheduler_vector));

	numNeigh = (int)(sqrt(scheduler_vector.size()));
	neighIdx = new int[numNeigh];
	maxLoadedIdx = -1;
	pollInterval = config->ws_poll_interval_start;
	chooseBitMap = new bool[scheduler_vector.size()];
	resetChooseBM();

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
	int maxLoad = -1000000;

	Package loadPkg;
	loadPkg.set_virtualpath("query load");
	for (int i = 0; i < numNeigh; i++)
	{
//		sendrecv(scheduler_vecotr.at[neighIdx[i]], config->scheduler_port_num, ***);
//		if (maxLoad < load)
//		{
//			maxLoad = load;
//			maxLoadedIdx = neighIdx[i];
//		}
	}
}



void* MatrixScheduler::workstealing(void*)
{
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

