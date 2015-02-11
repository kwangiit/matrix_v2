/* util.h
 *
 * general utility classes, functions, and
 * variables used in MATRIX
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 *      Email: kwang22@hawk.iit.edu
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <deque>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stropts.h>
#include <time.h>
#include <bits/time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "metazht.pb.h"
#include "metamatrix.pb.h"
#include "metatask.pb.h"
#include "ZHT/src/cpp_zhtclient.h"
#include "config.h"


using namespace std;

typedef map<long, vector<long> > adjList;	// task DAG represented as adjacency list
typedef map<long, long> inDegree;	// number of parents of each task

/* template of converting a number
 * to a string using stringstream */
template<typename T> string num_to_str(T num)
{
	stringstream ss;
	ss << num;
	return ss.str();
}

/* template of converting a string
 * to a number using stringstream */
template<typename T> T str_to_num(const string &str)
{
	stringstream ss(str);
	T num;
	return ss >> num ? num : 0;
}

extern uint _BUF_SIZE;	// the buffer size defined to receive messages

/* tokenize a string with a specified delimiter */
extern vector<string> tokenize(const string&, const char*);

/* get the ip address of a machine */
extern int get_ip(char*);

/* execute a command */
extern string exec(const char*);

/* get the identifier of a given host */
extern string get_host_id(const string&);

/* read a file line by line */
extern vector<string> read_from_file(const string&);

/* find the index of a string in a vector of strings */
extern int get_self_idx(const string&, vector<string>);

/* generate the adjacency list of a DAG */
extern void gen_dag_adjlist(adjList&, string&, long, long);

/* generate the infomation of
 * number of parents for each task */
extern void gen_dag_indegree(adjList&, inDegree&);

extern void gen_dag_parents(adjList&, adjList&);

/* get the current time of day in micro-second */
extern long get_time_usec();

/* get the current time of day in mili-second */
extern double get_time_msec();

/* get the current time of day in second */
extern double get_time_sec();

/* calculate the time duration between two times */
extern timespec time_diff(timespec, timespec);

extern string taskmsg_to_str(const TaskMsg &taskMsg);
extern TaskMsg str_to_taskmsg(const string &str);

extern string value_to_str(const Value &value);
extern Value str_to_value(const string &str);

extern string mm_to_str(const MatrixMsg &mm);
extern MatrixMsg str_to_mm(const string &str);

/* a mutex class for locking and uncloking */
class Mutex
{
	private:
		pthread_mutex_t mutex;
	public:
		Mutex();
		virtual ~Mutex();
		int lock();
		int unlock();
};

/* a Peer base class inheritended by both
 * MATRIX client and scheduler */
class Peer
{
	public:
		Peer(const string&);
		virtual ~Peer();

		bool init_zht_client(const string&, const string&);

		void wait_all_scheduler();
		void wait_all_task_recv();

		void set_id(const string&);

		string get_id(void);

		void set_index(int);

		int get_index(void);

		void incre_ZHT_msg_count(long);

		void insert_wrap(const string &key, const string &val);
		void insert_wrap(const char *key, const char *val);

		void lookup_wrap(const string &key, string &result);
		void lookup_wrap(const char *key, char *result);

		void send_batch_tasks(vector<TaskMsg>, int, const string&);
		void recv_batch_tasks(vector<TaskMsg>, int);

		void recv_batch_tasks();

		ZHTClient zc;
		Configuration *config;
		vector<string> schedulerVec;
		bool running;
		long numZHTMsg;

	private:
		string id;
		int index;
};

struct HighPriorityByDataSize
{
	bool operator()(const TaskMsg &hpTaskMsg,
			const TaskMsg &lpTaskMsg) const
	{
		return hpTaskMsg.datalength() > lpTaskMsg.datalength();
	}
};

extern Mutex tokenMutex;
extern Mutex sockMutex;
#endif /* UTIL_H_ */
