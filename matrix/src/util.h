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

typedef map<int, vector<int>> adj_list; 	// vertex ---> list of vertices
typedef map<int, int> in_degree; 		// vertex ---> Indegree of the vertex

//typedef pair<int, vector<int>> dag_value;	// mapping info of each vertex in DAG
//typedef map<int, dag_value> task_dag;	// vertex ---> indegree of vertex, adjlist in string format as above

extern int getInt(const string&);
extern long getLong(const string&);

extern int getIP(char*);
extern string exec(char*);
extern string getHostIdentity(const string&);
extern vector<string> readFromFile(const string&);
extern int getSelfIndex(const string&, vector<string>);

extern void genDagAdjlist(adj_list&, const string&, int, long);
extern void genDagInDegree(adj_list&, in_degree&);

extern bool initZHTClient(ZHTClient&, const string&, const string&);

class Mutex
{
	private:
		pthread_mutex_t mutex;
	public:
		Mutex();
		virtual ~Mutex();
		int Lock();
		int Unlock();
};
#endif /* UTIL_H_ */
