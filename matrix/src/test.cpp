/*
 * test.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "config.h"

int main(int args, char* argv[])
{
	if (args < 2)
	{
		cout << "check the input, we need a configuration file" << endl;
		return -1;
	}

	Configuration *config = new Configuration(argv[1]);

	cout << "start to print the Configuration parameters:" << endl;
	cout << "NumTaskPerClient\t" << config->numTaskPerClient << endl;//= getLong((string)config_map.find("NumTaskPerClient"));
	cout << "NumAllTask\t" << config->numAllTask << endl;//= getLong((string)config_map.find("NumAllTask"));
	cout << "NumCorePerExecutor\t" << config->numCorePerExecutor << endl;
	/*max_task_per_pkg = getLong((string)config_map.find("MaxTaskPerPkg"));
	monitor_interval = getLong((string)config_map.find("MonitorInterval"));
	scheduler_port_num = getLong((string)config_map.find("SchedulerPortNo"));
	sleep_lengh = getLong((string)config_map.find("SleepLength"));
	ws_poll_interval_start = getLong((string)config_map.find(
									"WorkStealPollIntervalStart"));
	ws_poll_interval_ub = getLong((string)config_map.find(
									"WorkStealPollIntervalUpperBound"));

	scheduler_memList_file = config_map.find("SchedulerMemlistFile");
	net_prot = config_map.find("NetworkProtocol");
	dag_type = config_map.find("DagType");
	submission_mode = config_map.find("SubmissionMode");
	workload_file = config_map.find("WorkloadFile");
	client_log = config_map.find("ClientLog");
	scheduler_log = config_map.find("SchedulerLog");
	zht_memList_file = config_map.find("ZhtMemlistFile");
	zht_config_file = config_map.find("ZhtConfigFile");

	num_core_per_executor = getInt((string)config_map.find("NumCorePerExecutor"));*/
	return 0;
}



