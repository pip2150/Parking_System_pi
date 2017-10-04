/*
 * Socket.h
 *
 *  Created on: 2017. 7. 25.
 *      Author: dhrco
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

struct sockaddr_in;

class Socket{

protected:

	sockaddr_in *addr;
	int sock;

public:

	static const int MAXHOSTNAME = 200;
	static const int MAXCONNECTIONS = 5;
	static const int MAXRECV = 500;

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

#endif
