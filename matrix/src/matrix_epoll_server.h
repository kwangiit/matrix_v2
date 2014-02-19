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

using namespace std;

class MatrixEventData
{
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

class MatrixEpollData
{
	public:
		MatrixEpollData(const int& fd, const sockaddr * const sender);
		virtual ~MatrixEpollData();

		int fd() const;
		const sockaddr * const sender() const;

	private:
		int _fd;
		const sockaddr *_sender;
};

class MatrixEpollServer
{
	public:
		MatrixEpollServer(const char*, const string&);
		virtual ~MatrixEpollServer();

		void serve();

	private:
		int create_and_bind(const char *port);
		int create_and_bind(const char *host, const char *port);
		int make_socket_non_blocking(const int& sfd);
		int makeSvrSocket();
		int reuseSock(int sock);
		void init_thread();

	private:
		static void* threadedServe(void *arg);

	private:
		MatrixEpollServer();

	private:
		char* _port;
		string _protoc;
		queue<MatrixEventData> _eventQueue;

	private:
		static const int MAX_EVENTS;
};

#endif /* MATRIX_EPOLL_SERVER_H_ */
