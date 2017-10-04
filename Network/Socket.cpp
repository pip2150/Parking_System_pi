#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cerrno>
#include "Socket.hpp"

Socket::Socket() {
	if((sock  = socket( PF_INET, SOCK_STREAM, 0)) == -1){
		std::cerr << "socket : "<<strerror(errno) << std::endl;
		exit(1);
	};
	addr = new sockaddr_in;
}

void Socket::setSocket(int sock) {
	this->sock = sock;
	addr = new sockaddr_in;
}

Socket::~Socket() {
	if(sock != -1)
		::close(sock);
}

bool Socket::isValid(){
	return (sock != -1);
}

bool Socket::send(std::string s) {
	if((::send(sock, s.c_str(), s.size(), MSG_NOSIGNAL)) == -1) {
		std::cerr << "send : "<<strerror(errno) << std::endl;
		return false;
	}
	else {
		return true;
	}
}

int Socket::recv(std::string& s) {
	int status;

	char buf[MAXRECV + 1];
	s = "";
	memset(buf, 0, MAXRECV + 1);

	if((status = ::recv(sock, buf, MAXRECV, 0)) == -1) {
		std::cerr << "recv : "<<strerror(errno) << std::endl;
		return -1;
	}
	else if(status == 0) {
		return 0;
	}
	else {
		s = buf;
		return status;
	}
}

void Socket::set_non_blocking(bool b) {
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

bool ServerSocket::create() {
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(!isValid())
		return false;

	int on = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,(char*)&on, sizeof(on)) == -1){
		std::cerr << "create : "<<strerror(errno) << std::endl;
		return false;
	}

	return true;
}

bool ServerSocket::bind(int port) {
	if(!isValid())
		return false;

	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(port);

	if((::bind(sock,(struct sockaddr*)addr, sizeof(*addr))) == -1){
		std::cerr << "bind : "<<strerror(errno) << std::endl;
		return false;
	}


	return true;
}


bool ServerSocket::listen() {
	if(!isValid())
		return false;

	if((::listen(sock, MAXCONNECTIONS)) == -1){
		std::cerr << "listen : "<<strerror(errno) << std::endl;
		return false;
	}

	return true;
}

bool ServerSocket::accept(Socket &clientsocket) {
	int addrLen = sizeof(*addr);

	int csock = ::accept(sock,(sockaddr *)addr, (socklen_t *)&addrLen);
	clientsocket.setSocket(csock);

	if(clientsocket.isValid()){
		return true;
	}

	std::cerr << "accept : "<<strerror(errno) << std::endl;
	return false;
}

bool ClientSocket::connect(std::string host,  int port) {
//	if(!isValid())
//		return false;

	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);

	int status = inet_pton(AF_INET, host.c_str(), &addr->sin_addr);

	//if(errno == EAFNOSUPPORT) return false;

	status = ::connect(sock,(sockaddr *)addr, sizeof(*addr));

	if(status == 0)
		return true;

	std::cerr << "connect : "<<strerror(errno) << std::endl;
	return false;
}
