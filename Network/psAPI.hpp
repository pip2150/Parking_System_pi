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

void enter(ClientSocket& sock, std::string host,std::string plateNum);
void exit(ClientSocket& sock, std::string host, std::string plateNum);
void parking(ClientSocket& sock, std::string host, int floor, std::string zoneName, int zoneIndex, std::string plateNum);

};

#endif /* PSAPI_HPP_ */
