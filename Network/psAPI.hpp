/*
 * psAPI.hpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#ifndef PSAPI_HPP_
#define PSAPI_HPP_
#include "Socket.hpp"

namespace psapi{

void carEnter(ClientSocket& sock, std::string host,std::string plateNum);
void carExit(ClientSocket& sock, std::string host, std::string plateNum);
void carParking(ClientSocket& sock, std::string host, int floor, std::string zoneName, int zoneIndex, std::string plateNum);

};

#endif /* PSAPI_HPP_ */
