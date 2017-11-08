/*
 * API.hpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#ifndef API_HPP_
#define API_HPP_

#include <iostream>

namespace sock{
class ClientSocket;
}

namespace ps {

class API {
protected:
	sock::ClientSocket *sock;		//!< @brief 서버 소켓
	int port;						//!< @brief 서버 포트 번호
	std::string hostname;			//!< @brief 서버 호스트 이름

public:
	API(std::string hostname, int port);

	virtual void inout(std::string plateStr, std::string inout) = 0;
	virtual void exit(std::string plateStr) = 0;
	virtual void enter(std::string plateStr) = 0;
	virtual bool resopnse() = 0;

	~API();
};

} /* namespace ps */

#endif /* API_HPP_ */
