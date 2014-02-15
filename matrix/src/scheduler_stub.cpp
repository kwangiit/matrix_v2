/*
 * scheduler_stub.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"

MatrixScheduler::MatrixScheduler(const string &config_file)
{
	config = new Configuration(config_file);
	//setHostname(getHostIdentity(config->host_identity_type));
	scheduler_vector = readFromFile(config->scheduler_memList_file);
	//setIndex(getSelfIndex(getHostname(), scheduler_vector));
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

void MatrixScheduler::workstealing(void*)
{

}

void MatrixScheduler::forkWSThread()
{
	if (config->work_stealing_on == 1)
	{
		pthread_t wsThread;
		while (pthread_create(&wsThread, NULL, ))
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

