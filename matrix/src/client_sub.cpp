/*
 * client_sub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_sub.h"

MatrixClient::MatrixClient(string config_file)
{
	config = new Configuration(config_file);
	setHostname(getHostIdentity(config->host_identity_type));
	host_vector = readFromFile(config->scheduler_memList_file);
	setIndex(getSelfIndex(getHostname(), host_vector));
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
