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
	set_id(get_host_id(config->hostIdType));
	schedulerVec = read_from_file(config->schedulerMemFile);
	taskVec = read_from_file(config->workloadFile);
	set_index(get_self_idx(get_id(), schedulerVec));
}

void MatrixClient::set_id(string id)
{
	this->id = id;
}

string MatrixClient::get_id()
{
	return id;
}

void MatrixClient::set_index(int index)
{
	this->index = index;
}

int MatrixClient::get_index()
{
	return index;
}

void MatrixClient::insert_taskinfo_to_zht(ZHTClient &zc,
		adjList &dagAdjList, inDegree &dagInDegree)
{
	for (adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		stringstream ss;
		ss << index << it->first;
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

void MatrixClient::wait_all_scheduler(ZHTClient &zc)
{
	string key("number of scheduler registered");
	stringstream ss;
	ss <<  schedulerVec.size();
	string expValue(ss.str());

	while (zc.state_change_callback(key, expValue, config->sleepLength) != 0)
	{
		usleep(1);
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
	if (config->submitMode.compare("best case"))
	{
		submit_task_bc();
	}
	else if (config->submitMode.compare("worst case"))
	{
		int toScheIdx = rand() % schedulerVec.size();
		submit_task_wc(taskVec, toScheIdx);
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
		numTaskLeft -= numTaskSendPerPkg;
	}
}

void* MatrixClient::monitoring(void* args)
{
	ZHTClient *zc = (ZHTClient*)args;
	string key("num tasks done");
	stringstream ss;
	ss << config->numAllTask;
	string expValue(ss.str());

	while (zc->state_change_callback(key, expValue, config->monitorInterval) != 0)
	{
		usleep(1);
	}
	return NULL;
}

void MatrixClient::do_monitoring(ZHTClient &zc)
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
