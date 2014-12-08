/*
 * matrix_epoll_server.h
 *
 *  Created on: Feb 18, 2014
 *      Author: kwang
 */

#ifndef MATRIX_EPOLL_SERVER_H_
#define MATRIX_EPOLL_SERVER_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <queue>
#include "scheduler_stub.h"

using namespace std;

class MatrixEventData {
public:
	MatrixEventData(int fd, const char* buf, size_t bufsize, sockaddr addr);

	~MatrixEventData();

	int fd() const;
	char* buf() const;
	size_t bufsize() const;
	sockaddr fromaddr();

private:
	int _fd;
	char* _buf;
	size_t _bufsize;
	sockaddr _fromaddr;
};

class MatrixEpollData {
public:
	MatrixEpollData(const int& fd, const sockaddr * const sender);
	virtual ~MatrixEpollData();

	int fd() const;
	const sockaddr * const sender() const;

private:
	int _fd;
	const sockaddr *_sender;
};

class MatrixEpollServer {
public:
	MatrixEpollServer(long, MatrixScheduler*);
	virtual ~MatrixEpollServer();

	void serve();

private:
	int create_and_bind(const char*);
	int create_and_bind(const char*, const char*);
	int make_socket_non_blocking(const int&);
	int make_svr_socket();
	int reuse_sock(int);
	void init_thread();

private:
	static void* threaded_serve(void*);

private:
	MatrixEpollServer();

private:
	MatrixScheduler *_ms;
	long _port;
	Mutex eqMutex;
	queue<MatrixEventData> _eventQueue;

private:
	static const int MAX_EVENTS;
};

#endif /* MATRIX_EPOLL_SERVER_H_ */
