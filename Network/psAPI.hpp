/*
 * psAPI.hpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#ifndef PSAPI_HPP_
#define PSAPI_HPP_

#include "Socket.hpp"
#include "Http.hpp"
#include <iostream>
#include <string>

#define HEADERNUM 8

namespace ps{
class API{
private:
	ClientSocket sock;
	std::string hostname;
	int port;
	std::string path;

	void setHeader(HeaderLine headerLine[], std::string content);

public:
	API();
	void inout(std::string plateNum, std::string inout);
	void enter(std::string plateNum);
	void exit(std::string plateNum);
	void parking(int floor, std::string zoneName, int zoneIndex, std::string plateNum);
	void resopnse();
};

};

#endif /* PSAPI_HPP_ */
