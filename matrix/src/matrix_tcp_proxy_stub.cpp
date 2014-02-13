/*
 * matrix_tcp_proxy_stub.cpp
 *
 *  Created on: Feb 13, 2014
 *      Author: kwang
 */

#include "matrix_tcp_proxy_stub.h"
#include "ZHT/src/bigdata_transfer.h"
#include "ZHT/src/Env.h"

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

MatrixTCPProxy::MatrixTCPProxy()
{

}

MatrixTCPProxy::~MatrixTCPProxy()
{

}

int MatrixTCPProxy::mmakeClientSocket(const string &host, const uint &port)
{
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
	dest.sin_port = htons(port);

	struct hostent * hinfo = gethostbyname(host.c_str());
	if (hinfo == NULL)
	{
		cerr << "MatrixTCPProxy::mmakeClientSocket(): ";
		herror(host.c_str());
		return -1;
	}

	memcpy(&dest.sin_addr, hinfo->h_addr, sizeof(dest.sin_addr));

	int to_sock = socket(PF_INET, SOCK_STREAM, 0); //try to change here

	if (to_sock < 0)
	{

		cerr << "MatrixTCPProxy::mmakeClientSocket(): error on ::socket(...): "
				<< strerror(errno) << endl;
		return -1;
	}

	int ret_con = connect(to_sock, (struct sockaddr *) &dest, sizeof(sockaddr));

	if (ret_con < 0)
	{

		cerr << "MatrixTCPProxy::mmakeClientSocket(): error on ::connect(...): "
				<< strerror(errno) << endl;
		return -1;
	}

	return to_sock;
}

#ifdef BIG_MSG
int MatrixTCPProxy::msendTo(int to_sock, const void *sendbuf, int sendcount)
{
	BdSendBase *pbsb = new BdSendToServer((char*) sendbuf);
	int sentSize = pbsb->bsend(to_sock);
	delete pbsb;
	pbsb = NULL;

	//prompt errors
	if (sentSize < sendcount)
	{
		cerr << "MatrixTCPProxy::msendTo(): error on BdSendToServer::bsend(...): "
		 << strerror(errno) << endl;
	}

	return sentSize;
}
#endif

#ifdef SML_MSG
int MatrixTCPProxy::msendTo(int to_sock, const void *sendbuf, int sendcount)
{

	int sentSize = ::send(to_sock, sendbuf, sendcount, 0);

	//prompt errors
	if (sentSize < sendcount)
	{

		cerr << "MatrixTCPProxy::msendTo(): error on socket send(...): "
		<< strerror(errno) << endl;
	}

	return sentSize;
}
#endif

#ifdef BIG_MSG
int MatrixTCPProxy::mrecvFrom(int from_sock, void *recvbuf)
{
	string result;
	int recvcount = loopedrecv(from_sock, result);

	memcpy(recvbuf, result.c_str(), result.size());

	//prompt errors
	if (recvcount < 0)
	{
		cerr << "MatrixTCPProxy::mrecvFrom(): error on mloopedrecv(...): "
		<< strerror(errno) << endl;
	}

	return recvcount;
}
#endif

#ifdef SML_MSG
int MatrixTCPProxy::mrecvFrom(int from_sock, void *recvbuf)
{
	char buf[Env::BUF_SIZE];
	memset(buf, '\0', sizeof(buf));

	int recvcount = ::recv(from_sock, buf, sizeof(buf), 0);

	memcpy(recvbuf, buf, strlen(buf));

	//prompt errors
	if (recvcount < 0)
	{
		cerr << "MatrixTCPProxy::mrecvFrom(): error on ::recv(...): "
		<< strerror(errno) << endl;
	}

	memset(buf, '\0', sizeof(buf));

	return recvcount;
}
#endif

int MatrixTCPProxy::mloopedrecv(int sock, string &srecv)
{
	ssize_t recvcount = -2;
	socklen_t addr_len = sizeof(struct sockaddr);

	BdRecvBase *pbrb = new BdRecvFromServer();

	char buf[Env::BUF_SIZE];

	while (1)
	{
		memset(buf, '\0', sizeof(buf));
		ssize_t count;
		count = ::recv(sock, buf, sizeof(buf), 0);

		if (count == -1 || count == 0)
		{
			recvcount = count;
			break;
		}

		bool ready = false;

		string bd = pbrb->getBdStr(sock, buf, count, ready);

		if (ready)
		{
			srecv = bd;
			recvcount = srecv.size();
			break;
		}
		memset(buf, '\0', sizeof(buf));
	}

	delete pbrb;
	pbrb = NULL;

	return recvcount;
}

bool MatrixTCPProxy::msendrecv(const void *sendbuf, const size_t sendcount,
		void *recvbuf, size_t &recvcount)
{

}
