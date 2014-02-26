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
	/* check whether specified a configuration file */
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\tconfiguration_file!\n");
		exit(-1);
	}

	string configFileStr(argv[1]);

	/* create a MATRIX scheduler*/
	MatrixScheduler ms = new MatrixScheduler(configFileStr);

	ms.fork_es_thread();	// forks the epoll event driven server

	ms.regist();	// regists to ZHT

	ms.wait_all_scheduler();	// waits all the other schedulers are running

	ms.fork_crt_thread();	// forks checking ready task thread

	ms.fork_exec_task_thread();	// forks executing task threads

	ms.fork_cct_thread();	// forks checking complete task thread

	ms.fork_ws_thread();	// forks work stealing thread

	ms.fork_record_stat_thread();	// forks recording status thread
}
