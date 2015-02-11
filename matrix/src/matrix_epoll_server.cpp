/*
 * matrix_epoll_server.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: kwang
 */
#include "util.h"
#include "matrix_epoll_server.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

MatrixEventData::MatrixEventData(int fd, const char* buf, size_t bufsize,
		sockaddr addr) {
	_fd = fd;
	int len = strlen((const char*) buf);
	_buf = (char*) calloc(len + 1, sizeof(char));
	memcpy(_buf, buf, len + 1);

	_bufsize = bufsize;
	_fromaddr = addr;
}

MatrixEventData::~MatrixEventData() {

}

int MatrixEventData::fd() const {
	return _fd;
}

char* MatrixEventData::buf() const {
	return _buf;
}

size_t MatrixEventData::bufsize() const {
	return _bufsize;
}

sockaddr MatrixEventData::fromaddr() {
	return _fromaddr;
}

MatrixEpollData::MatrixEpollData(const int& fd, const sockaddr * const sender) :
		_fd(fd), _sender(sender) {

}

MatrixEpollData::~MatrixEpollData() {
	if (_sender != NULL) {
		free((void*) _sender);
		_sender = NULL;
	}
}

int MatrixEpollData::fd() const {
	return _fd;
}

const sockaddr* const MatrixEpollData::sender() const {
	return _sender;
}

const int MatrixEpollServer::MAX_EVENTS = 64;

MatrixEpollServer::MatrixEpollServer(long port, MatrixScheduler *ms) :
		_port(port), _ms(ms), _eventQueue() {
	eqMutex = Mutex();
}

MatrixEpollServer::~MatrixEpollServer() {

}

int MatrixEpollServer::create_and_bind(const char *port) {

	return create_and_bind(NULL, port);
}

int MatrixEpollServer::create_and_bind(const char *host, const char *port) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	s = getaddrinfo(host, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			/* We managed to bind successfully! */
			break;
		}

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo(result);

	return sfd;
}

int MatrixEpollServer::make_socket_non_blocking(const int& sfd) {
	int flags, s;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

int MatrixEpollServer::make_svr_socket() {
	long port = _port;
	struct sockaddr_in svrAdd_in; /* socket info about our server */
	int svrSock = -1;

	try {
		memset(&svrAdd_in, 0, sizeof(struct sockaddr_in)); /* zero the struct before filling the fields */
		svrAdd_in.sin_family = AF_INET; /* set the type of connection to TCP/IP */
		svrAdd_in.sin_addr.s_addr = INADDR_ANY; /* set our address to any interface */
		svrAdd_in.sin_port = htons(port); /* set the server port number */

		if (_ms->config->netProtoc.compare("TCP") == 0) //make socket
				{
			svrSock = socket(AF_INET, SOCK_STREAM, 0); /* OS will return a fd for network stream connection*/
		} else //UDP
		{
			svrSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		}

		if (svrSock <= 0) {
			printf("Error occurred when creating the socket:"
					"%d to the server port:%ld\n", svrSock, port);
			printf("%s\n", strerror(errno));

			close(svrSock);
			return -1;
		}

		if (bind(svrSock, (struct sockaddr*) &svrAdd_in,
				sizeof(struct sockaddr)) < 0) {
			printf("Error occurred binding the socket:%d "
					"to the server port:%ld\n", svrSock, port);
			printf("%s", strerror(errno));

			close(svrSock);
			return -1;
		}

		if (_ms->config->netProtoc.compare("TCP") == 0) //TCP needs listen, UDP does not.
				{
			/* start listening, allowing a queue of up to 1 pending connection */
			if (listen(svrSock, SOMAXCONN) < 0) {
				printf("Error occurred while enabling listen "
						"on the socket:%d\n", svrSock);
				printf("%s", strerror(errno));

				close(svrSock);
				return -1;
			}
		}
	} catch (exception &e) {
		fprintf(stderr, "exception caught:\n\t%s", e.what());
	}

	return svrSock;
}

int MatrixEpollServer::reuse_sock(int sock) {
	int reuse_addr = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
			sizeof(reuse_addr));
	if (ret < 0) {
		cerr << "resuse socket failed: " << strerror(errno) << endl;
		return -1;
	} else
		return 0;
}

/*int MatrixEpollServer::serve_request(int client_sock,
 void *buff, sockaddr fromAddr)
 {
 Package pkg;
 pkg.ParseFromArray(buff, _BUF_SIZE);

 string msg = pkg.virtualpath();
 if (msg.)
 return -1;
 }*/

void* MatrixEpollServer::threaded_serve(void *arg) {
	MatrixEpollServer *mes = (MatrixEpollServer*) arg;

	while (true) {
		while (!mes->_eventQueue.empty()) {
			mes->eqMutex.lock();
			MatrixEventData eventData = mes->_eventQueue.front();
			mes->_eventQueue.pop();
			mes->eqMutex.unlock();
			mes->_ms->proc_req(eventData.fd(), eventData.buf());
		}
	}

	return NULL;
}

void MatrixEpollServer::init_thread() {
	pthread_t thread;
	while (pthread_create(&thread, NULL, threaded_serve, this) != 0) {
		sleep(1);
	}
}

void MatrixEpollServer::serve() {
	init_thread();

	int sfd, s;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;

	sfd = make_svr_socket();
	if (sfd == -1)
		abort();

	/* The event loop */
	while (1) {
		sockaddr *in_addr = (sockaddr *) calloc(1, sizeof(struct sockaddr));
		socklen_t in_len = sizeof(struct sockaddr);

		int infd = accept(sfd, in_addr, &in_len);
		if (infd == -1) {
			free(in_addr);
		} else {
			char *buf = (char*) calloc(_BUF_SIZE, sizeof(char));
			int count = recv(infd, buf, _BUF_SIZE, 0);
			if (count > 0) {
				MatrixEventData eventData(infd, buf, sizeof(buf), *in_addr);
				eqMutex.lock();
				_eventQueue.push(eventData);
				eqMutex.unlock();
			}
			if (buf == NULL) {
				free(buf);
				buf = NULL;
			}
		}
	}
}
