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
		MatrixClient(string);
		virtual ~MatrixClient();

		void insertTaskInfoToZHT(ZHTClient&, adj_list&, in_degree&);
		void waitAllScheduler(ZHTClient&);
		void initTask(void);
		void submitTask(void);
		void submitTaskBC(void);
		void submitTaskWC(const vector<string>&, int);
		void *monitoring(void*);
		void doMonitoring(ZHTClient&);

		void setHostname(string);
		string getHostname(void);
		void setIndex(int);
		int getIndex(void);

		Configuration *config;
		vector<string> scheduler_vector;
		vector<string> task_vector;

	private:
		string hostname;
		int index;
};


#endif /* CLIENT_SUB_H_ */
