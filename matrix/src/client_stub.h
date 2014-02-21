/*
 * client_sub.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CLIENT_SUB_H_
#define CLIENT_SUB_H_

#include <sstream>

#include <netdb.h>
#include <pthread.h>
#include <error.h>

#include "config.h"

class MatrixClient
{
	public:
		MatrixClient(const string&);
		virtual ~MatrixClient();

		void insert_taskinfo_to_zht(ZHTClient&, adjList&, inDegree&);
		void wait_all_scheduler(ZHTClient&);
		void init_task(void);
		void submit_task(void);
		void submit_task_bc(void);
		void submit_task_wc(const vector<string>&, int);
		void *monitoring(void*);
		void do_monitoring(ZHTClient&);

		void set_id(string);
		string get_id(void);
		void set_index(int);
		int get_index(void);

		Configuration *config;
		vector<string> schedulerVec;
		vector<string> taskVec;

	private:
		string id;
		int index;
};

#endif /* CLIENT_SUB_H_ */
