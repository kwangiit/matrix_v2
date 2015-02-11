/*
 * net.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: kwang
 */
#include "matrix_tcp_proxy_stub.h"
#include <pthread.h>
#include <string.h>

int create_sock(const string &ip, long port) {
	int to_sock;
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	const char *ipChar = ip.c_str();
	struct hostent * hinfo = gethostbyname(ipChar);

	if (hinfo == NULL) {
		printf("getbyname failed!\n");
		return -1;
	}

	dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
	dest.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
	dest.sin_port = htons(port);
	to_sock = socket(PF_INET, SOCK_STREAM, 0); //try change here.................................................
	if (to_sock < 0) {
		cerr << "TCPProxy::makeClientSocket(): error on ::socket(...):"
				<< to_sock << endl;
		return -1;
	}
	int ret_con = connect(to_sock, (struct sockaddr *) &dest, sizeof(sockaddr));
	if (ret_con < 0) {
		cerr << "TCPProxy::makeClientSocket(): error on ::connect(...):"
				<< ret_con << endl;
		return -1;
	}
	return to_sock;
}

int send_first(const string &ip, long port, const string &buf) {
	int to_sock = create_sock(ip, port);
	if (to_sock != -1) {
		send_bf(to_sock, buf);
	}
	return to_sock;
//
//	struct sockaddr_in dest;
//	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
//	const char *ipChar = ip.c_str();
//	struct hostent * hinfo = gethostbyname(ipChar);
//
//	if (hinfo == NULL) {
//		printf("getbyname failed!\n");
//		return -1;
//	}
//
//	dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
//	dest.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
//	dest.sin_port = htons(port);
//	to_sock = socket(PF_INET, SOCK_STREAM, 0); //try change here.................................................
//	if (to_sock < 0) {
//		cerr << "TCPProxy::makeClientSocket(): error on ::socket(...):"
//				<< to_sock << endl;
//		return -1;
//	}
//	int ret_con = connect(to_sock, (struct sockaddr *) &dest, sizeof(sockaddr));
//	if (ret_con < 0) {
//		cerr << "TCPProxy::makeClientSocket(): error on ::connect(...):"
//				<< ret_con << endl;
//		return -1;
//	}
//
//	send_bf(to_sock, buf);
//
//	return to_sock;
}

int send_bf(int sock, const string &buf) {
	return send(sock, buf.data(), buf.length(), 0);
}

int recv_bf(int sock, string &buf) {
	char bufStr[_BUF_SIZE];
	memset(bufStr, '\0', sizeof(bufStr));

	int ret = recv(sock, bufStr, sizeof(bufStr), 0);
	buf.assign(bufStr);

	return ret;
}

int send_big(int sock, const string &buf) {
	int length = buf.length();
	int numSent = 0;
	string str;

	while (length >= _BUF_SIZE) {
		str = buf.substr(numSent, _BUF_SIZE);
		send_bf(sock, str);
		numSent += _BUF_SIZE;
		length -= _BUF_SIZE;
	}

	if (length > 0) {
		str = buf.substr(numSent, length);
		str.append("$");
	} else {
		str.assign("$");
	}
	send_bf(sock, str);
	numSent += str.length();

	return numSent;
}

int recv_big(int sock, string &buf) {
	int count = recv_mul(sock, buf);
	buf = buf.substr(0, buf.length() - 1);
	return count;
}

int send_mul(int sock, const string &buf, bool end) {
	string bufUp;
	bufUp.append(buf);
	bufUp.append("##");

	if (end) {
		bufUp.append("$");
	}
	return send_bf(sock, bufUp);
}

int recv_mul(int sock, string &buf) {
	string tmpBuf;
	int count = recv_bf(sock, tmpBuf);
	int sum = 0;

	while (count > 0) {
		sum += count;
		buf.append(tmpBuf);
		if (tmpBuf[tmpBuf.length() - 1] == '$') {
			break;
		}
		count = recv_bf(sock, tmpBuf);
	}
	return sum;
}

//int send_mul(int sock, const string &buf)
//{
//	char *cStr;
//	cStr = (char*)calloc((buf.length() + 5 + 1), sizeof(char));
//
//	int i = 0;
//
//	for(i = 0; i < 5; i++)
//	{
//		cStr[i] = '#';
//	}
//
//	string lenStr = num_to_str<int>(buf.length());
//
//	for(i = 0; i < lenStr.length(); i++)
//	{
//		cStr[i] = lenStr[i];
//	}
//
//	cout << "The size is:" << lenStr.length() << ", and " << cStr << endl;
//
//	i = 5;
//	memcpy(&cStr[i], buf.c_str(), buf.length() + 1);
//
//	int len = i + buf.length();
//
//	int sendLength = send(sock, cStr, len, 0);
//
//	cout << "The string after is:" << cStr << endl;
//
//	if (cStr != NULL)
//	{
//		free(cStr);
//		cStr = NULL;
//	}
//	return sendLength;
//}
//
//int recv_mul(int sock, string &bufStr)
//{
//	char buf[_BUF_SIZE];
//	char tempbuf[_BUF_SIZE];
//	memset(buf, '\0', _BUF_SIZE);
//	memset(tempbuf, '\0', _BUF_SIZE);
//
//	int numByte = recv(sock, tempbuf, sizeof(tempbuf), 0);
//	cout << "The stuff received is:" << tempbuf << ", and the number of bytes is:" << numByte << endl;
//
//	if (numByte > 0)
//	{
//		int string_size, all_size;
//
//		const int header_size = 5;
//		char header[header_size];
//		memcpy(header, tempbuf, header_size);
//		stringstream size_stream;
//		size_stream << header;
//		string str;
//		size_stream >> str;
//		vector<string> vec = tokenize(str, "#");
//		string_size = str_to_num<int>(vec[0]);
//		//size_stream >> string_size;
//
//		all_size = string_size + header_size;
//
//		int pos = 0;
//		int bytes_recd = numByte;
//		memcpy(&buf[pos], tempbuf, numByte);
//		pos = pos + numByte;
//
//		while (bytes_recd < all_size)
//		{
//			memset(tempbuf, '\0', _BUF_SIZE);
//			numByte = recv(sock, tempbuf, sizeof(tempbuf), 0); //cout << "Received size = " << count << endl;
//			cout << "OK, The stuff received is:" << tempbuf << endl;
//
//			if (numByte > 0)
//			{
//				memcpy(&buf[pos], tempbuf, numByte);
//				pos = pos + numByte;
//				bytes_recd = bytes_recd + numByte;
//			}
//		}
//
//		bufStr.assign(&buf[header_size], string_size);
//		cout << "The buffer string is:" << bufStr << endl;
//		return all_size;
//	}
//
//	return -1;
//}

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
