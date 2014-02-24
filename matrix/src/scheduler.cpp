/*
 * scheduler.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"

using namespace std;

int main(int argc, char *argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\t"
						 "configuration_file!\n");
		exit(-1);
	}

	string configFileStr(argv[1]);

	MatrixScheduler ms = new MatrixScheduler(configFileStr);

	ms.fork_es_thread();

	ms.regist();

	ms.wait_all_scheduler();

	ms.fork_crt_thread();

	ms.fork_exec_task_thread();

	ms.fork_cct_thread();

	ms.fork_ws_thread();

	ms.fork_record_stat_thread();
}
