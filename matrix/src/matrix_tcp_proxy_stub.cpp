/*
 * net.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: kwang
 */
#include "matrix_tcp_proxy_stub.h"
#include <pthread.h>
#include <string.h>

int send_first(const string &ip, long port, const string &buf)
{
	int to_sock;
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	struct hostent * hinfo = gethostbyname(ip.c_str());

	if (hinfo == NULL)
	{
		printf("getbyname failed!\n");
		return -1;
	}

	dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
	dest.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
	dest.sin_port = htons(port);
	to_sock = socket(PF_INET, SOCK_STREAM, 0); //try change here.................................................
	if (to_sock < 0)
	{
		return -1;
	}
	int ret_con = connect(to_sock, (struct sockaddr *) &dest,
			sizeof(sockaddr));
	if (ret_con < 0)
	{
		return -1;
	}
	send_bf(to_sock, buf);

	return to_sock;
}

int send_bf(int sock, const string &buf)
{
	const char *bufStr = buf.c_str();
	int bufSize = strlen(bufStr);
	return send(sock, bufStr, bufSize, 0);
}

int recv_bf(int sock, string &buf)
{
	char bufStr[_BUF_SIZE];
	memset(bufStr, _BUF_SIZE, '\0');

	int ret = recv(sock, bufStr, _BUF_SIZE, 0);
	buf.assign(bufStr);

	return ret;
}

//void* es_proc(void *arg)
//{
//	MatrixEpollServer *ms = (MatrixEpollServer*)arg;
//	ms->serve();
//	pthread_exit(NULL);
//	return NULL;
//}

//pthread_t create_es_thread(char *port, char *type)
//{
//	printf("Now, I am creating a epoll server!\n");
//	pthread_t esThread;
//	MatrixEpollServer *ms = new MatrixEpollServer(port, type);
//
//	while (pthread_create(&esThread, NULL, es_proc, (void*)ms) != 0)
//	{
//		sleep(1);
//	}
//
//	return esThread;
//}
