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

Configuration::Configuration(string config_file)
{
	parseConfig(config_file);
}

Configuration::~Configuration()
{

}

void Configuration::parseConfig(string config_file)
{
	map<string, string> config_map;
	fstream file_stream(config_file.c_str());

	if (!file_stream.good())
	{
		return;
	}

	string line, key, value;

	while (getline(file_stream, line))
	{
		stringstream ss(line);
		ss >> key >> value;
		if (!key.empty() && key[0] != '#')
		{
			config_map.insert(make_pair(key, value));
		}
		key.clear();
		value.clear();
	}

	file_stream.close();

	num_task_per_client = getLong(config_map.find("NumTaskPerClient")->second);
	num_all_task = getLong(config_map.find("NumAllTask")->second);
	max_task_per_pkg = getLong(config_map.find("MaxTaskPerPkg")->second);
	monitor_interval = getLong(config_map.find("MonitorInterval")->second);
	scheduler_port_num = getLong(config_map.find("SchedulerPortNo")->second);
	sleep_lengh = getLong(config_map.find("SleepLength")->second);
	ws_poll_interval_start = getLong(config_map.find("WorkStealPollIntervalStart")->second);
	ws_poll_interval_ub = getLong(config_map.find("WorkStealPollIntervalUpperBound")->second);

	scheduler_memList_file = config_map.find("SchedulerMemlistFile")->second;
	net_prot = config_map.find("NetworkProtocol")->second;
	dag_type = config_map.find("DagType")->second;
	host_identity_type = config_map.find("HostIdentityType")->second;
	submission_mode = config_map.find("SubmissionMode")->second;
	workload_file = config_map.find("WorkloadFile")->second;
	client_log = config_map.find("ClientLog")->second;
	scheduler_log = config_map.find("SchedulerLog")->second;
	zht_memList_file = config_map.find("ZhtMemlistFile")->second;
	zht_config_file = config_map.find("ZhtConfigFile")->second;

	num_core_per_executor = getInt(config_map.find("NumCorePerExecutor")->second);
}
