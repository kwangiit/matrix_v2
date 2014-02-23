/*
 * Configuration.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

Configuration::Configuration(const string &configFile)
{
	parse_config(configFile);
}

Configuration::~Configuration()
{

}

void Configuration::parse_config(const string &configFile)
{
	map<string, string> configMap;
	fstream fileStream(configFile.c_str());

	if (!fileStream.good())
	{
		return;
	}

	string line, key, value;

	while (getline(fileStream, line))
	{
		stringstream ss(line);
		ss >> key >> value;
		if (!key.empty() && key[0] != '#')
		{
			configMap.insert(make_pair(key, value));
		}
		key.clear();
		value.clear();
	}

	fileStream.close();

	numTaskPerClient = str_to_num<long>(configMap.find("NumTaskPerClient")->second);
	numAllTask = str_to_num<long>(configMap.find("NumAllTask")->second);
	numCorePerExecutor = str_to_num<int>(configMap.find("NumCorePerExecutor")->second);
	maxTaskPerPkg = str_to_num<long>(configMap.find("MaxTaskPerPkg")->second);
	monitorInterval = str_to_num<long>(configMap.find("MonitorInterval")->second);
	schedulerPortNo = str_to_num<long>(configMap.find("SchedulerPortNo")->second);
	sleepLength = str_to_num<long>(configMap.find("SleepLength")->second);
	workStealingOn = str_to_num<int>(configMap.find("WorkStealOn")->second);
	wsPollIntervalStart = str_to_num<long>(configMap.find("WorkStealPollIntervalStart")->second);
	wsPollIntervalUb = str_to_num<long>(configMap.find("WorkStealPollIntervalUpperBound")->second);

	schedulerMemFile = configMap.find("SchedulerMemlistFile")->second;
	netProtoc = configMap.find("NetworkProtocol")->second;
	dagType = configMap.find("DagType")->second;
	dagArg = str_to_num<int>(configMap.find("DagArgument")->second);
	hostIdType = configMap.find("HostIdentityType")->second;
	submitMode = configMap.find("SubmissionMode")->second;
	workloadFile = configMap.find("WorkloadFile")->second;
	clientLog = configMap.find("ClientLog")->second;
	taskLog = configMap.find("TaskLog")->second;
	systemLog = configMap.find("SystemLog")->second;
	schedulerLog = configMap.find("SchedulerLog")->second;
	zhtMemFile = configMap.find("ZhtMemlistFile")->second;
	zhtConfigFile = configMap.find("ZhtConfigFile")->second;
}
