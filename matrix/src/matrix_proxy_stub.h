/*
 * matrix_proxy_stub.h
 *
 *  Created on: Feb 13, 2014
 *      Author: kwang
 */

#ifndef MATRIX_PROXY_STUB_H_
#define MATRIX_PROXY_STUB_H_

#include <sys/types.h>

class MatrixProtoAddr
{
	public:
		MatrixProtoAddr();
		MatrixProtoAddr(const MatrixProtoAddr&);
		virtual ~MatrixProtoAddr();

		int fd;
		void *sender;
};


#endif /* MATRIX_PROXY_STUB_H_ */
