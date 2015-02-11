/*
 * scheduler.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"

using namespace std;

int main(int argc, char *argv[]) {
	/* check whether specified a configuration file */
	if (argc != 2) {
		fprintf(stderr, "The usage is: scheduler\tconfiguration_file!\n");
		exit(-1);
	}

	string configFileStr(argv[1]);

	/* create a MATRIX scheduler*/
	MatrixScheduler *ms = new MatrixScheduler(configFileStr);

	ms->regist();	// regists to ZHT

	ms->wait_all_scheduler();	// waits all the other schedulers are running

	ms->fork_es_thread();	// forks the epoll event driven server

//	ms->load_data();

//	ms->get_task_from_file();

	ms->wait_all_task_recv();

	clock_gettime(0, &(ms->start));

	ms->fork_crt_thread();	// forks checking ready task thread

	ms->fork_exec_task_thread();	// forks executing task threads

	ms->fork_cct_thread();	// forks checking complete task thread

	ms->fork_ws_thread();	// forks work stealing thread

	if (ms->config->policy.compare("FLWS") == 0) {
		ms->fork_localQueue_monitor_thread();
	}

	ms->fork_record_stat_thread();	// forks recording status thread

	while (1) {
		sleep(1);
	}
}
