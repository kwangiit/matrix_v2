/*
 * client_sub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"

MatrixClient::MatrixClient(string config_file)
{
	config = new Configuration(config_file);
	setHostname(getHostIdentity(config->host_identity_type));
	scheduler_vector = readFromFile(config->scheduler_memList_file);
	task_vector = readFromFile(config->workload_file);
	setIndex(getSelfIndex(getHostname(), scheduler_vector));
	setNumAllTask(config->num_all_task);
	setNumTaskPerClient(config->num_task_per_client);
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

void MatrixClient::setNumAllTask(long num_all_task)
{
	this->num_all_task = num_all_task;
}

long MatrixClient::getNumAllTask()
{
	return num_all_task;
}

void MatrixClient::setNumTaskPerClient(long num_task_per_client)
{
	this->num_task_per_client = num_task_per_client;
}

long MatrixClient::getNumTaskPerClient()
{
	return num_task_per_client;
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

void MatrixClient::submitTask()
{
	if (config->submission_mode.compare("best case"))
	{

	}
	else if (config->submission_mode.compare("worst case"))
	{
		long numTaskLeft = getNumTaskPerClient();
		long numTaskSent = config->max_task_per_pkg;

		while (numTaskLeft > 0)
		{
			if (numTaskLeft <= config->max_task_per_pkg)
			{
				numTaskSent = numTaskLeft;
			}
			string tasks;
			for (int i = 0; i < numTaskSent; i++)
			{
				tasks += this->task_vector.at(i + (
						config->num_task_per_client - numTaskLeft));
				tasks += "<part>";
			}

		}
	}
}
