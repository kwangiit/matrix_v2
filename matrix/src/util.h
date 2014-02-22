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
#include <map>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stropts.h>
#include <time.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "metazht.pb.h"
#include "ZHT/src/cpp_zhtclient.h"

using namespace std;

typedef map<int, vector<int>> adjList;	// task DAG represented as adjacency list
typedef map<int, int> inDegree;	// number of parents of each task

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

extern uint _Buf_SIZE;	// the buffer size defined to receive messages

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
extern void gen_dag_adjlist(adjList&, const string&, int, long);

/* generate the infomation of
 * number of parents for each task */
extern void gen_dag_indeg(adjList&, inDegree&);

/* initialize as a ZHT client*/
extern bool init_zht_client(ZHTClient&, const string&, const string&);

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
		void wait_all_scheduler(ZHTClient&);
		void set_id(string);
		string get_id(void);
		void set_index(int);
		int get_index(void);

		Configuration *config;
		vector<string> schedulerVec;

	private:
		string id;
		int index;
};
#endif /* UTIL_H_ */
