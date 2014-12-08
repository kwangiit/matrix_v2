/*
 * Configuration.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

Configuration::Configuration(const string &configFile) {
	parse_config(configFile);
}

Configuration::~Configuration() {

}

void Configuration::parse_config(const string &configFile) {
	map<string, string> configMap;
	fstream fileStream(configFile.c_str());

	if (!fileStream.good())
		return;

	string line, key, value;

	while (getline(fileStream, line)) {
		stringstream ss(line);
		ss >> key >> value;
		if (!key.empty() && key[0] != '#')
			configMap.insert(make_pair(key, value));
		key.clear();
		value.clear();
	}

	fileStream.close();

	numTaskPerClient = str_to_num<long>(configMap.find("NumTaskPerClient")->second);

	//numFile = str_to_num<int>(configMap.find("NumFile")->second);

	//locality = str_to_num<double>(configMap.find("Locality")->second);

	numAllTask = str_to_num<long>(configMap.find("NumAllTask")->second);

	numMapTask = str_to_num<long>(configMap.find("NumMapTask")->second);

	numReduceTask = str_to_num<long>(configMap.find("NumReduceTask")->second);

	numCorePerExecutor = str_to_num<int>(configMap.find("NumCorePerExecutor")->second);

	maxTaskPerPkg = str_to_num<long>(configMap.find("MaxTaskPerPkg")->second);

	monitorInterval = str_to_num<long>(configMap.find("MonitorInterval")->second);

	schedulerPortNo = str_to_num<long>(configMap.find("SchedulerPortNo")->second);

	sleepLength = str_to_num<long>(configMap.find("SleepLength")->second);

	workStealingOn = str_to_num<int>(configMap.find("WorkStealOn")->second);

	wsPollIntervalStart = str_to_num<long>(configMap.find("WorkStealPollIntervalStart")->second);

	wsPollIntervalUb = str_to_num<long>(configMap.find("WorkStealPollIntervalUpperBound")->second);

	policy = configMap.find("Policy")->second;

	dataSizeThreshold = str_to_num<long>(configMap.find("DataSizeThreshold")->second);

	estTimeThreadshold = str_to_num<long>(configMap.find("EstimatedTimeThreshold")->second);

	schedulerMemFile = configMap.find("SchedulerMemlistFile")->second;

	netProtoc = configMap.find("NetworkProtocol")->second;

	dagType = configMap.find("DagType")->second;

	dagArg = str_to_num<long>(configMap.find("DagArgument")->second);

	hostIdType = configMap.find("HostIdentityType")->second;

	submitMode = configMap.find("SubmissionMode")->second;

	workloadFile = configMap.find("WorkloadFile")->second;

	schedulerWorkloadPath = configMap.find("SchedulerWorkloadPath")->second;

	clientLog = str_to_num<int>(configMap.find("ClientLog")->second);

	taskLog = str_to_num<int>(configMap.find("TaskLog")->second);

	systemLog = str_to_num<int>(configMap.find("SystemLog")->second);

	schedulerLog = str_to_num<int>(configMap.find("SchedulerLog")->second);

	zhtMemFile = configMap.find("ZhtMemlistFile")->second;

	zhtConfigFile = configMap.find("ZhtConfigFile")->second;
}
