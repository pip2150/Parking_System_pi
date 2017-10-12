/*
 * socket.hpp
 *
 *  Created on: 2017. 7. 25.
 *      Author: dhrco
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <iostream>

struct sockaddr_in;

/**
	@brief 소켓통신을 위한 네임스페이스
 */
namespace sock{

/**
	@brief 소켓 클래스
 */
class Socket{

protected:

	sockaddr_in *addr;		//!< @brief 소켓 주소를 지정하는 구조체
	int sock;				//!< @brief 소켓 파일 기술자

public:

	static const int MAXHOSTNAME = 200;			//!< @brief hostname 의 최대 길이
	static const int MAXCONNECTIONS = 5;		//!< @brief 최대 연결 가능한 연결 수
	static const int MAXRECV = 500;				//!< @brief recv 시 받는 버퍼의 크기

	/**
		@overload
		@brief Socket 클래스를 초기화하는 생성자
	 */
	Socket();

	/**
		@overload
		@brief 파일 기술자를 통해 Socket 클래스를 초기화하는 생성자
		@param sock 초기화할 파일 기술자
	 */
	Socket(int sock);

	/**
		@brief Socket 클래스의 소멸자
	 */
	~Socket();

	/**
		@brief 소켓을 통해 데이터를 보내는 메소드
		@param data 소켓을 통해 보낼 데이터
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool send(const std::string data);

	/**
		@brief 소켓으로 부터 데이터를 받는 메소드
		@param data 소켓으로 부터 받은 데이터
		@return 성공 시 받은 데이터의 크기을 반환, 실패 시 -1을 반환
	 */
	int recv(std::string *data);

	/**
		@brief 소켓이 올바른지 검사하는 메소드
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool isValid();

	/**
		@brief nonblocking I/O 설정
		@param b true 시 nonblocking false 시 blocking
	 */
	void set_non_blocking(bool b);
};


/**
	@brief 클라이언트 소켓
 */
class ClientSocket : public Socket {

public:

	/**
		@brief 소켓을 서버와 연결하는 메소드
		@param host 연결할 서버의 주소
		@param port 사용할 포트 번호
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool connect(const std::string host, const int port);

};

/**
	@brief 서버 소켓
 */
class ServerSocket : public Socket {
public:

	/**
		@brief 서버 소켓을 create 하는 메소드
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool create();

	/**
		@brief 서버 소켓을 bind 하는 메소드
		@param port port
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool bind(const int port);

	/**
		@brief 서버 소켓을 listen 하는 메소드
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool listen();

	/**
		@brief 클라이언트 소켓을  accept 하는 메소드
		@param clientSocket accept 할 클라이언트의 소켓
		@return 성공 시 true 그렇지 않으면 false
	 */
	bool accept(Socket* clientSocket);

};

};

#endif
