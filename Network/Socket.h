/*
 * Socket.h
 *
 *  Created on: 2017. 7. 25.
 *      Author: dhrco
 */


#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket{
protected:
	sockaddr_in addr;
	int sock;

public:
	Socket();
	void setSocket(int);
	virtual ~Socket();

	bool send(std::string);
	int recv(std::string&);
	bool isValid();

	void set_non_blocking(bool);
};

class ClientSocket : public Socket {
public:
	bool connect(std::string host, int port);
};

class ServerSocket : public Socket {
public:
	bool create();
	bool bind(int port);
	bool listen();
	bool accept(Socket&);
};
