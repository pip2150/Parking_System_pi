/*
 * Socket.cpp
 *
 *  Created on: 2017. 7. 25.
 *      Author: dhrco
 */

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include "Socket.hpp"

sock::Socket::Socket() {

	addr = new sockaddr_in;

	if((sock  = ::socket( PF_INET, SOCK_STREAM, 0)) != -1)
		return ;

	perror("socket");
	exit(1);

}

sock::Socket::Socket(int sock){

	this->sock = sock;
	addr = new sockaddr_in;

}

sock::Socket::~Socket() {

	if(sock != -1)
		::close(sock);

}

bool sock::Socket::isValid(){

	return (sock != -1);

}

bool sock::Socket::send(std::string data) {

	if((::send(sock, data.c_str(), data.size(), MSG_NOSIGNAL)) != -1)
		return true;

	perror("send");
	return false;

}

int sock::Socket::recv(std::string *data) {

	int status;

	char buf[MAXRECV + 1];
	*data = "";

	for(int i=0;i<MAXRECV+1;i++)
		buf[i] = 0;

	if((status = ::recv(sock, buf, MAXRECV, 0)) == -1) {
		perror("recv");
		return -1;
	}
	else if(status == 0) {
		return 0;
	}
	else {
		*data = buf;
		return status;
	}

}

void sock::Socket::set_non_blocking(bool b) {

	int opts;

	opts = fcntl(sock, F_GETFL);

	if(opts < 0){
		return;
	}

	if(b)
		opts =(opts | O_NONBLOCK);
	else
		opts =(opts & ~O_NONBLOCK);

	fcntl(sock, F_SETFL, opts);

}

bool sock::ServerSocket::create() {

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(!isValid())
		return false;

	int on = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,(char*)&on, sizeof(on)) == -1)
		return true;

	perror("create");
	return false;

}

bool sock::ServerSocket::bind(int port) {

	if(!isValid())
		return false;

	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(port);

	if((::bind(sock,(struct sockaddr*)addr, sizeof(*addr))) != -1)
		return true;

	perror("bind");
	return false;

}

bool sock::ServerSocket::listen() {

	if(!isValid())
		return false;

	if((::listen(sock, MAXCONNECTIONS)) != -1)
		return true;

	perror("listen");
	return false;

}

bool sock::ServerSocket::accept(Socket *clientSocket) {

	int addrLen = sizeof(*addr);

	int csock = ::accept(sock,(sockaddr *)addr, (socklen_t *)&addrLen);
	clientSocket = new Socket(csock);

	if(clientSocket->isValid()){
		return true;
	}

	perror("accept");
	return false;

}

bool sock::ClientSocket::connect(std::string host,  int port) {

	if(host.length() > MAXHOSTNAME){
		std::cerr << "connect : Hostname is too long" <<std::endl;
		exit(1);
	}

	if(!isValid())
		return false;

	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);

	int status = inet_pton(AF_INET, host.c_str(), &addr->sin_addr);

	if(status == -1){
		perror("connect");
		exit(1);
	}

	if(status == 0){
		std::cerr << "connect : Valid Network address" <<std::endl;
		exit(1);
	}

	status = ::connect(sock,(sockaddr *)addr, sizeof(*addr));

	if(status != -1)
		return true;

	perror("connect");
	return false;

}
