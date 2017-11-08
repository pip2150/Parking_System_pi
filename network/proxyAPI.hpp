/*
 * ProxyAPI.hpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#ifndef PROXYAPI_HPP_
#define PROXYAPI_HPP_

#include "api.hpp"

namespace ps {

class ProxyAPI: public API {
public:
	ProxyAPI(std::string hostname, int port);

	virtual void inout(std::string plateStr, std::string inout);

	virtual void exit(std::string plateStr);

	virtual void enter(std::string plateStr);

	virtual bool resopnse();

	bool resopnse(std::string *dst);

	void print();
};

} /* namespace ps */

#endif /* PROXYAPI_HPP_ */
