/*
 * client_sub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"
#include "ZHT/src/meta.pb.h"

MatrixClient::MatrixClient(const string
		&configFile):Peer(configFile)
{
	clock_gettime(0, &start);

	taskVec = read_from_file(config->workloadFile);

	string base;
	base.append(num_to_str<int>(schedulerVec.size()));
	base.append("_");
	base.append(num_to_str<long>(config->numTaskPerClient));

	string indexStr = num_to_str<int>(get_index());
	string suffix = base + "_" + indexStr;

	if (config->clientLog.compare("yes") == 0 && get_index() == 0)
	{
		string clientLogFile("./client_" + suffix);
		clientLogOS.open(clientLogFile.c_str());
	}
	if (config->taskLog.compare("yes") == 0 && get_index() == 0)
	{
		string taskLogFile("./task_" + suffix);
		taskLogOS.open(taskLogFile.c_str());
	}
	if (config->systemLog.compare("yes") == 0 && get_index() == 0)
	{
		string systemLogFile("./system_" + suffix);
		systemLogOS.open(systemLogFile.c_str());
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

	cout << "I am a Matrix Client, it takes me " << diff.tv_sec << "seconds, "
			"and " << diff.tv_nsec << " nanosecond for initialization!" << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "I am a Matrix Client, it takes me " << diff.tv_sec << "seconds, "
				"and " << diff.tv_nsec << " nanosecond for initialization!" << endl;
	}
}

void MatrixClient::insert_taskinfo_to_zht(ZHTClient &zc,
		adjList &dagAdjList, inDegree &dagInDegree)
{
	cout << "--------------------------------"
			"----------------------------" << endl;
	cout << "Now, I am going to insert task information to ZHT" << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "-------------------------------"
				"-----------------------------" << endl;
		clientLogOS << "Now, I am going to insert "
				"task information to ZHT" << endl;
	}

	clock_gettime(0, &start);

	for (adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		stringstream ss;
		ss << get_index() << it->first;
		string taskId(ss.str());
		vector<int> existList = it->second;
		int inDegree = dagInDegree[it->first];
		Value value;
		value.set_id(taskId);
		value.set_indegree(inDegree);
		for (int i = 0; i < existList.size(); i++)
		{
			stringstream ssChild;
			ssChild << index << existList.at(i);
			string sChild(ssChild.str());
			value.add_children(sChild);
		}
		value.set_nummove(0);
		value.set_history("");
		value.set_submittime(0);
		value.set_arrivetime(0);
		value.set_rqueuedtime(0);
		value.set_exetime(0);
		value.set_fintime(0);
		string seriValue = value.SerializeAsString();
		zc.insert(taskId, seriValue);
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

	cout << "I am done, the time taken is:" << diff.tv_sec
			<< " s, and " << diff.tv_nsec + " ns" << endl;
	cout << "--------------------------------"
				"----------------------------" << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "I am done, the time taken is:" << diff.tv_sec
					<< " s, and " << diff.tv_nsec + " ns" << endl;
		clientLogOS << "--------------------------------"
					"----------------------------" << endl;
	}
}

void MatrixClient::init_task()
{
	int numTask = config->numTaskPerClient;
	for (int i = 0; i < numTask; i++)
	{
		stringstream ss;
		ss << index << i;
		string taskId(ss.str());
		taskVec.at(i) = taskId + " " + taskVec.at(i);
	}
}

void MatrixClient::submit_task()
{
	cout << "--------------------------------"
						"----------------------------" << endl;
	cout << "Now, I am going to submit tasks to the schedulers" << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
		clientLogOS << "Now, I am going to submit "
				"tasks to the schedulers" << endl;
	}

	clock_gettime(0, &start);

	if (config->submitMode.compare("best case") == 0)
	{
		submit_task_bc();
	}
	else if (config->submitMode.compare("worst case") == 0)
	{
		int toScheIdx = rand() % schedulerVec.size();
		submit_task_wc(taskVec, toScheIdx);
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

	cout << "It took " << diff.tv_sec << "s, and "
			<< diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
						"----------------------------" << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "It took " << diff.tv_sec << "s, and "
				<< diff.tv_nsec << " ns" << endl;
		clientLogOS << "--------------------------------"
						"----------------------------" << endl;
	}
}

void MatrixClient::submit_task_bc()
{
	int toScheIdx = -1, numSche = schedulerVec.size();
	vector<vector<string>> taskVecs(numSche);

	for (int i = 0; i < config->numTaskPerClient; i++)
	{
		toScheIdx = i % numSche;
		taskVecs.at(toScheIdx).push_back(taskVec.at(i));
	}

	for (int i = 0; i < numSche; i++)
	{
		submit_task_wc(taskVecs.at(i), i);
	}
}

void MatrixClient::submit_task_wc(const vector<string> &taskVec, int toScheIdx)
{
	long numTaskLeft = taskVec.size();
	long numTaskBeenSent = 0;
	long numTaskSendPerPkg = config->maxTaskPerPkg;

	while (numTaskLeft > 0)
	{
		if (numTaskLeft <= config->maxTaskPerPkg)
		{
			numTaskSendPerPkg = numTaskLeft;
		}
		string tasks;
		numTaskBeenSent = config->numTaskPerClient - numTaskLeft;
		for (int i = 0; i < numTaskSendPerPkg; i++)
		{
			tasks += taskVec.at(i + numTaskBeenSent);
			tasks += "eot";
		}
		Package taskPkg;
		taskPkg.set_virtualpath("send task");
		taskPkg.set_realfullpath(tasks);
		string taskPkgStr = taskPkg.SerializeAsString();
		// send the taskPkgStr to the server scheduler_vector.at(toScheIdx)
		// and receive acks
		numTaskLeft -= numTaskSendPerPkg;
	}
}

void* MatrixClient::monitoring(void *args)
{
	ZHTClient *zc = (ZHTClient*)args;
	string key("num tasks done");

	long numAllCores = config->numCorePerExecutor * schedulerVec.size();
	long numIdleCores = 0;
	long numTaskWait = 0, numTaskReady = 0;
	long preNumTaskDone = 0, numTaskDone = 0;
	double prevTimeUs = 0.0, currentTimeUs = 0.0, instantThr = 0.0;

	//string expValue = num_to_str<long>(config->numAllTask);
	string numTaskFinStr;
	bool systemLog = false;
	if (systemLogOS.is_open())
	{
		systemLog = true;
		systemLogOS << "Time(us)\tNumAllCore\tNumIdleCore\t"
				"NumTaskWait\tNumTaskReady\tNumTaskDone\tThroughput" << endl;
	}

	while (1)
	{
		currentTimeUs = get_time_usec();
		zc->lookup(key, numTaskFinStr);
		numTaskDone = str_to_num<long>(numTaskFinStr);
		if (systemLog)
		{
			for (int i = 0; i < )
		}
		usleep(config->monitorInterval);
	}

	while (zc->state_change_callback(key, expValue, config->monitorInterval) != 0)
	{
		usleep(1);
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

	double time = (double)diff.tv_sec + (double)diff.tv_nsec / 1E9;
	double throughput = config->numAllTask / time;

	cout << "It takes " << diff.tv_sec << "s, and " << diff.tv_nsec <<
			" ns to finish " << config->numAllTask << " tasks" << endl;
	cout << "The overall throughput is:" << throughput << endl;
	if (clientLogOS.is_open())
	{
		clientLogOS << "It takes " << diff.tv_sec << "s, and " <<
				diff.tv_nsec << " ns to finish " << config->numAllTask
				<< " tasks" << endl;
		clientLogOS << "The overall throughput is:" << throughput << endl;
		clientLogOS.flush();
		clientLogOS.close();
	}

	return NULL;
}

void MatrixClient::do_monitoring(ZHTClient &zc)
{
	if (index != 0)
	{
		return;
	}

	pthread_t monThread;

	while (pthread_create(&monThread, NULL, monitoring, &zc) != 0)
	{
		sleep(1);
	}

	pthread_join(monThread, NULL);
}
