/*
 * psAPI.hpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#ifndef SERVERAPI_HPP_
#define SERVERAPI_HPP_

#include <iostream>

#include "api.hpp"

namespace http{
struct HeaderLine;
}

/**
	@brief 주차장 관리 시스템
 */
namespace ps{

/**
	@brief 주차장의 차량 정보를 서버와 통신하기 위한 api
 */
class ServerAPI : public API{

private:

	int HEADERSIZE = 8;				//!< @brief 서버에 보낼 HTTP 메세지의 헤더의 갯수
	std::string path;				//!< @brief HTML 문서를 저장할 경로

	/**
		@brief 서버에 보낼 HTTP 메세지의 헤더를 정의하는 메소드
		@param headerLine 정의할 헤더들의 배열
		@param content HTTP 메세지의 Body
	 */
	void setHeader(http::HeaderLine headerLine[], std::string content);

public:

	/**
		@brief ServerAPI 클래스를 초기화하는 생성자
		@param hostname 서버 호스트 이름
		@param port 서버 포트 번호
	 */
	ServerAPI(std::string hostname, int port);

	/**
		@brief 주차장에 차량 출입시에 번호판 텍스트를 서버에 보내는 메소드
		@param plateStr 서버에 보낼 번호판 문자열
		@param inout 입, 출 여부의 문자열 "/enter" or "/exit"
	 */
	virtual void inout(std::string plateStr, std::string inout);

	/**
		@brief 주차장에 들어오는 차량의 번호판 텍스트를 서버에 보내는 메소드
		@param plateStr 서버에 보낼 번호판 문자열 @see inout
	 */
	virtual void enter(std::string plateStr);

	/**
		@brief 주차장을 나가는 차량의 번호판 텍스트를 서버에 보내는 메소드
		@param plateStr 서버에 보낼 번호판 문자열 @see inout
	 */
	virtual void exit(std::string plateStr);

	/**
		@brief 주차 시 주차 정보를 서버에 보내는 메소드
		@param floor 주차장의 층
		@param zoneName 주차 구역 이름
		@param zoneIndex 주차 구역 번호
		@param plateStr 서버에 보낼 번호판 문자열
	 */
	void parking(int floor, std::string zoneName, int zoneIndex, std::string plateStr);

	/**
		@brief 서버에 보낸 메시지에 대한 응답를 html 문서로 내보내는 메소드
	 */
	virtual bool resopnse();
};

};

#endif /* SERVERAPI_HPP_ */
