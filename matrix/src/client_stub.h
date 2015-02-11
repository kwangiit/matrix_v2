/*
 * client_stub.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CLIENT_SUB_H_
#define CLIENT_STUB_H_

#include <sstream>

#include <pthread.h>
#include <error.h>

#include "matrix_tcp_proxy_stub.h"

class MatrixClient: public Peer {
public:
	MatrixClient(const string&);
	virtual ~MatrixClient();

	/* insert task information to ZHT */
	void insert_taskinfo_to_zht(adjList&, inDegree&);
	//void insert_taskinfo_to_zht(adjList&, adjList&);

	/* initialize tasks by adding taskId for each task */
	void init_task(void);

	/* submit all the tasks to schedulers */
	void submit_task(void);

	/* the best case senario where tasks are submitted
	 * in a interleaved load balanced way
	 * */
	void submit_task_bc(void);

	/* the worst case senario where tasks are submitted
	 * to just one random chosen scheduler
	 * */
	void submit_task_wc(vector<TaskMsg>, int);

	//void split_task_one(vector<string>, int);

	/* monitoring thread function */
	//void *monitoring(void*);
	/* fork a monitoring thread */
	void do_monitoring();

	vector<string> taskVec;	// vectors to store all the tasks of this client
	vector<TaskMsg> tasks;

	timespec start, end;	// start and end time stamps

	ofstream clientLogOS;	// client log output stream
	//ofstream taskLogOS;	// individual task log output stream
	ofstream systemLogOS;	// system status log output stream
};

#endif /* CLIENT_STUB_H_ */
