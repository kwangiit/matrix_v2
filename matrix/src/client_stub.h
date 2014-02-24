/*
 * client_sub.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CLIENT_SUB_H_
#define CLIENT_STUB_H_

#include <sstream>

#include <netdb.h>
#include <pthread.h>
#include <error.h>

#include "config.h"

class MatrixClient:public Peer
{
	public:
		MatrixClient(const string&);
		virtual ~MatrixClient();

		void insert_taskinfo_to_zht(adjList&, inDegree&);
		void init_task(void);
		void submit_task(void);
		void submit_task_bc(void);
		void submit_task_wc(const vector<string>&, int);
		void *monitoring(void*);
		void do_monitoring();

		vector<string> taskVec;
		timespec start, end;
		ofstream clientLogOS;
		ofstream taskLogOS;
		ofstream systemLogOS;
};

#endif /* CLIENT_STUB_H_ */
