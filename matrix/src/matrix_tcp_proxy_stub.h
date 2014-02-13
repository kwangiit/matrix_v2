/*
 * matrix_tcp_proxy_stub.h
 *
 *  Created on: Feb 13, 2014
 *      Author: kwang
 */

#ifndef MATRIX_TCP_PROXY_STUB_H_
#define MATRIX_TCP_PROXY_STUB_H_

#include "matrix_proxy_stub.h"

using namespace std;

class MatrixTCPProxy
{
	public:
		MatrixTCPProxy();
		virtual ~MatrixTCPProxy();

		int mmakeClientSocket(const string&, const uint&);
		int msendTo(int sock, const void*, int);
		int mrecvFrom(int sock, void* recvbuf);
		int mloopedrecv(int sock, string &srecv);
		bool msendrecv(const void *sendbuf, const size_t sendcount,
				void *recvbuf, size_t &recvcount);
		bool mteardown();
};

class MatrixTCPStub
{
	public:
		MatrixTCPStub();
		virtual ~MatrixTCPStub();

		int msendBack(MatrixProtoAddr addr, const void* sendbuf,
					int sendcount) const;
		bool mrecvsend(MatrixProtoAddr addr, const void *recvbuf);
};

#endif /* MATRIX_TCP_PROXY_STUB_H_ */
