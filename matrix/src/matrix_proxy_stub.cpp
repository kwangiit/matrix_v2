/*
 * matrix_proxy_stub.cpp
 *
 *  Created on: Feb 13, 2014
 *      Author: kwang
 */

#include "matrix_proxy_stub.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

MatrixProtoAddr::MatrixProtoAddr()
{
	fd = -1;
	sender = NULL;
}

MatrixProtoAddr::MatrixProtoAddr(const MatrixProtoAddr& addr)
{
	fd = addr.fd;
	sender = calloc(1, sizeof(sockaddr));
	memcpy(sender, &addr.sender, sizeof(sockaddr));
}

MatrixProtoAddr::~MatrixProtoAddr()
{
	free(sender);
}


