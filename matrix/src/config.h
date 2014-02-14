/*
 * Configure.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <fstream>
#include "util.h"

using namespace std;

class Configuration
{
	public:
		Configuration(const string&);
		virtual ~Configuration();

		void parseConfig(const string&);

		long num_task_per_client;
		long num_all_task;
		long max_task_per_pkg;
		long monitor_interval;
		long scheduler_port_num;
		long sleep_lengh;
		long ws_poll_interval_start;
		long ws_poll_interval_ub;

		string scheduler_memList_file;
		string net_prot;
		string dag_type;
		int dag_argu;
		string host_identity_type;
		string submission_mode;
		string workload_file;
		string client_log;
		string scheduler_log;
		string zht_memList_file;
		string zht_config_file;

		int num_core_per_executor;
};


#endif /* CONFIGURE_H_ */
