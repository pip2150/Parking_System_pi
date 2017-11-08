/*
 * ProxyAPI.cpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#include "proxyAPI.hpp"
#include "socket.hpp"

using namespace std;

namespace ps {

ProxyAPI::ProxyAPI(std::string hostname, int port) : API(hostname, port) {
}

void ProxyAPI::inout(std::string plateStr, std::string inout) {
	string buff = inout + plateStr;

	if(!sock->send(buff))
		::exit(1);
}

void ProxyAPI::exit(std::string plateStr){
	inout(plateStr,"exit@");
}

void ProxyAPI::enter(std::string plateStr){
	inout(plateStr,"entr@");
}

void ProxyAPI::print(){
	string buff = "prnt@";

	if(!sock->send(buff))
		::exit(1);
}

bool ProxyAPI::resopnse(){

	return ProxyAPI::resopnse(NULL);

}

bool ProxyAPI::resopnse(std::string *dst){
	string buff;

	sock->recv(&buff);

	if(buff == "fail")
		return false;

	if(dst != NULL)
		*dst = buff;

	return true;
}

} /* namespace ps */
