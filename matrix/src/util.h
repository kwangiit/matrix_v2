/*
 * util.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
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

typedef map<int, vector<int>> adjList; 	// vertex ---> list of vertices
typedef map<int, int> inDegree; 		// vertex ---> Indegree of the vertex

template<typename T> string num_to_str(T num)
{
	stringstream ss;
	ss << num;
	return ss.str();
}

template<typename T> T str_to_num(string &str)
{
	stringstream ss(str);
	T num;
	return ss >> num ? num : 0;
}

extern uint _Buf_SIZE;

extern vector<string> tokenize(const string&, const char*);
extern int get_ip(char*);
extern string exec(const char*);
extern string get_host_id(const string&);
extern vector<string> read_from_file(const string&);
extern int get_self_idx(const string&, vector<string>);
extern void gen_dag_adjlist(adjList&, const string&, int, long);
extern void gen_dag_indeg(adjList&, inDegree&);
extern bool init_zht_client(ZHTClient&, const string&, const string&);

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
#endif /* UTIL_H_ */
