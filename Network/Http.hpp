/*
 * Http.hpp
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#ifndef HTTP_HPP_
#define HTTP_HPP_

#include <iostream>

/**
	@brief HTTP 메시지를 다루는 네임 스페이스
 */
namespace http{

/**
	@brief Message 를 구성하는 헤더
 */
struct HeaderLine {
	std::string field;			//!< @brief 필드명
	std::string value;			//!< @brief 값
};

/**
	@brief ResponseMessge 의 상태 라인
 */
struct StatusLine{
	std::string version;		//!< @brief HTTP 버전
	std::string status;			//!< @brief 상태 코드
	std::string message;		//!< @brief 상태 메세지
};

/**
	@brief RequestMessge 의 요청 라인
 */
struct RequestLine{
	std::string method;			//!< @brief HTTP 메소드
	std::string url;			//!< @brief 요청 URL
	std::string version;		//!< @brief HTTP 버전
};

/**
	@brief HTTP 메세지
 */
class Message {
	const static int MAXHEADERS = 100;		//!< @brief 헤더의 최대 갯수

protected:

	const static int FIRSTLINESIZE = 3;		//!< @brief 메세지의 첫줄의 토큰 갯수
	std::string firstLine[FIRSTLINESIZE];	//!< @brief 첫줄 토큰들

private :

	HeaderLine* header;					//!< @brief 헤더
	std::string messageBody;				//!< @brief 메세지의 Body
	int headerSize;							//!< @brief 헤더의 갯수

	/**
		@brief HTTP 메세지를 크기에 맞게 파싱하는 메소드
		@param input 파싱할 문자열
		@param key 각 토큰을 구분할 키 문자열
		@param output 파싱된 결과 생성된 토큰
		@param size 출력되는 토큰의 크기
	 */
	void parser(std::string input, std::string key, std::string output[], int size);

public:

	/**
		@overload
		@brief HTTP 메세지의 문자열을 입력 받아 Message 를 만드는 생성자
		@param msg HTTP 메세지의 문자열
	 */
	Message(std::string msg);

	/**
		@overload
		@brief HTTP 메세지의 첫줄과 파싱된 HeaderLine 과 Body를 이용하여 Message 를 만드는 생성자
		@param firstLine HTTP 메세지의 첫번째 줄 ( 상태 라인이나 요청 라인을 나타냄 )
		@param headerLine HTTP 메세지의 헤더 라인들
		@param headerSize 헤더 라인의 갯수
		@param content HTTP 메세지의 Body
	 */
	Message(std::string firstLine[], HeaderLine headerLine[], int headerSize, std::string content);

	/**
		@brief index 번째의 헤더를 불러오는 메소드
		@param index 불러올 헤더의 인덱스 번호
		@return index 번째의 헤더
	 */
	HeaderLine getHeader(int index);

	/**
		@brief HTTP 메세지의 Body를 불러 오는 메소드
		@return HTTP 메세지의 Body
	 */
	std::string getMessageBody();

	/**
		@brief HTTP 메세지를 구성하는 헤더들의 갯수를 불러오는 메소드
		@return 헤더들의 갯수
	 */
	int getHeaderSize();

	/**
		@brief 파싱된 문자열을 HTTP 메시지로 불러오는 메소드
		@return 문자열로 이루어진 HTTP 메세지
	 */
	std::string getString();

};

/**
	@brief HTTP 응답 메시지
 */

class ResponseMessge : public Message{

public:

	/**
		@overload
		@brief HTTP 응답 메세지의 문자열을 입력 받아 ResponseMessage 를 만드는 생성자
		@param msg HTTP 응답 메세지의 문자열
	 */
	ResponseMessge(std::string msg);

	/**
		@overload
		@brief HTTP 응답 메세지의 첫줄, 파싱된 HeaderLine , Body를 이용하여 ResponseMessage 를 만드는 생성자
		@param statusCode HTTP 응답 메세지의 상태 라인
		@param headerLine HTTP 응답 메세지의 헤더 라인들
		@param headerSize 헤더 라인의 갯수
		@param content HTTP 응답 메세지의 Body
	 */
	ResponseMessge(StatusLine statusCode, HeaderLine headerLine[], int headerSize, std::string content);

	/**
		@brief HTTP 응답 메세지의 상태 라인을 불러오는 메소드
		@return HTTP 응답 메세지의 상태 라인
	 */
	StatusLine getStatusLine();

};

/**
	@brief HTTP 요청 메시지
 */
class RequestMessge : public Message{

public :

	/**
		@overload
		@brief HTTP 요청 메세지의 문자열을 입력 받아 RequestMessge 를 만드는 생성자
		@param msg HTTP 요청 메세지의 문자열
	 */
	RequestMessge(std::string msg);

	/**
		@overload
		@brief HTTP 요청 메세지의 첫줄, 파싱된 HeaderLine , Body를 이용하여 RequestMessge 를 만드는 생성자
		@param requestLine HTTP 요청 메세지의 상태 라인
		@param headerLine HTTP 요청 메세지의 헤더 라인들
		@param headerSize 헤더 라인의 갯수
		@param content HTTP 요청 메세지의 Body
	 */
	RequestMessge(RequestLine requestLine, HeaderLine headerLine[], int headerSize, std::string content);


	/**
		@brief HTTP 요청 메세지의 요청 라인을 불러오는 메소드
		@return HTTP 요청 메세지의 요청 라인
	 */
	RequestLine getRequestLine();

};
}

#endif
