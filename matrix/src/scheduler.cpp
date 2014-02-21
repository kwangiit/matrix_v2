/*
 * scheduler.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"

using namespace std;

ZHTClient zc;

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

	ms.regist(zc);

	ms.wait_all_scheduler(zc);

	ms.fork_crt_thread(zc);

	ms.fork_exec_task_thread();

	ms.fork_cct_thread(zc);

	ms.fork_ws_thread();

	ms.fork_record_stat_thread(zc);
}
