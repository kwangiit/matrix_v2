/*
 * client_sub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"
#include "ZHT/src/meta.pb.h"

MatrixClient::MatrixClient(const string &config_file)
{
	config = new Configuration(config_file);
	setHostname(getHostIdentity(config->host_identity_type));
	scheduler_vector = readFromFile(config->scheduler_memList_file);
	task_vector = readFromFile(config->workload_file);
	setIndex(getSelfIndex(getHostname(), scheduler_vector));
}

void MatrixClient::setHostname(string hostname)
{
	this->hostname = hostname;
}

string MatrixClient::getHostname()
{
	return hostname;
}

void MatrixClient::setIndex(int index)
{
	this->index = index;
}

int MatrixClient::getIndex()
{
	return index;
}

void MatrixClient::insertTaskInfoToZHT(ZHTClient &zc,
		adj_list &dag_adj_list, in_degree &dag_in_degree)
{
	for (adj_list::iterator it = dag_adj_list.begin();
						it != dag_adj_list.end(); ++it)
	{
		stringstream ss;
		ss << index << it->first;
		string taskId(ss.str());
		vector<int> existList = it->second;
		int inDegree = dag_in_degree[it->first];
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
		value.set_nummove(1);
		value.set_history("");
		value.set_submittime(0);
		value.set_arrivetime(0);
		value.set_rqueuedtime(0);
		value.set_exetime(0);
		value.set_fintime(0);
		string seriValue = value.SerializeAsString();
		zc.insert(taskId, seriValue);
	}
}

void MatrixClient::waitAllScheduler(ZHTClient &zc)
{
	string key("number of scheduler registered");
	stringstream ss;
	ss <<  scheduler_vector.size();
	string expValue(ss.str());

	while (zc.state_change_callback(key, expValue, config->sleep_lengh) != 0)
	{
		usleep(1);
	}
}

void MatrixClient::initTask()
{
	int numTask = config->num_task_per_client;
	for (int i = 0; i < numTask; i++)
	{
		stringstream ss;
		ss << index << i;
		string taskId(ss.str());
		task_vector.at(i) = taskId + "->" + task_vector.at(i);
	}
}

void MatrixClient::submitTask()
{
	if (config->submission_mode.compare("best case"))
	{
		submitTaskBC();
	}
	else if (config->submission_mode.compare("worst case"))
	{
		int toScheIdx = rand() % scheduler_vector.size();
		submitTaskWC(task_vector, toScheIdx);
	}
}

void MatrixClient::submitTaskBC()
{
	int toScheIdx = -1, numSche = scheduler_vector.size();
	vector<vector<string>> task_vectors(numSche);

	for (int i = 0; i < config->num_task_per_client; i++)
	{
		toScheIdx = i % numSche;
		task_vectors.at(toScheIdx).push_back(task_vector.at(i));
	}

	for (int i = 0; i < numSche; i++)
	{
		submitTaskWC(task_vectors.at(i), i);
	}
}

void MatrixClient::submitTaskWC(const vector<string> &taskVec, int toScheIdx)
{
	long numTaskLeft = taskVec.size();
	long numTaskBeenSent = 0;
	long numTaskSendPerPkg = config->max_task_per_pkg;

	while (numTaskLeft > 0)
	{
		if (numTaskLeft <= config->max_task_per_pkg)
		{
			numTaskSendPerPkg = numTaskLeft;
		}
		string tasks;
		numTaskBeenSent = config->num_task_per_client - numTaskLeft;
		for (int i = 0; i < numTaskSendPerPkg; i++)
		{
			tasks += taskVec.at(i + numTaskBeenSent);
			tasks += "eot";
		}
		Package taskPkg;
		taskPkg.set_virtualpath("send tasks");
		taskPkg.set_realfullpath(tasks);
		string taskPkgStr = taskPkg.SerializeAsString();
		// send the taskPkgStr to the server scheduler_vector.at(toScheIdx)
		numTaskLeft -= numTaskSendPerPkg;
	}
}

void* MatrixClient::monitoring(void* args)
{
	ZHTClient *zc = (ZHTClient*)args;
	string key("num tasks done");
	stringstream ss;
	ss << config->num_all_task;
	string expValue(ss.str());

	while (zc->state_change_callback(key, expValue, config->monitor_interval) != 0)
	{
		usleep(1);
	}
	return NULL;
}

void MatrixClient::doMonitoring(ZHTClient &zc)
{
	if (index != 1)
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
