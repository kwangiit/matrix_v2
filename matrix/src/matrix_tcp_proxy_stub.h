/*
 * net.h
 *
 *  Created on: Feb 27, 2014
 *      Author: kwang
 */

#ifndef NET_H_
#define NET_H_

#include "util.h"

using namespace std;

extern int send_first(const string&, long, const string&);
extern int send_bf(int, const string&);
extern int recv_bf(int, string&);

extern int send_big(int, const string&);
extern int recv_big(int, string&);
extern int send_mul(int, const string&, bool);
extern int recv_mul(int, string&);

extern int create_sock(const string&, long);

//extern pthread_t create_es_thread(char*, char*);


#endif /* NET_H_ */
