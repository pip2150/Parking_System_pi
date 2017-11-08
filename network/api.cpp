/*
 * API.cpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#include "api.hpp"

#include <cstring>
#include "socket.hpp"

namespace ps {

API::API(std::string hostname, int port){
	this->hostname = hostname;
	this->port = port;
	sock = new sock::ClientSocket();

    if(!sock->connect(hostname, port))
        std::exit(1);
}

API::~API() {

	delete sock;

}

} /* namespace ps */
