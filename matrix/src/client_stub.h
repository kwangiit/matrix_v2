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
		void initTask(void);
		void submitTask(void);

		void setHostname(string);
		string getHostname(void);
		void setIndex(int);
		int getIndex(void);
		void setNumAllTask(long);
		long getNumAllTask(void);
		void setNumTaskPerClient(long);
		long getNumTaskPerClient(void);

		Configuration *config;
		vector<string> host_vector;

	private:
		string hostname;
		int index;
		long num_all_task;
		long num_task_per_client;
};


#endif /* CLIENT_SUB_H_ */
