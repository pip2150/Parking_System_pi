/*
 * HttpParser.h
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#ifndef HTTP_HPP_
#define HTTP_HPP_

#include <string>
#include <vector>
#include <cstring>
#include <iostream>

class HttpParser {
private:
	std::vector<std::string> line;
	std::string header;
	std::string centent;
	std::string msg;
	enum method{GET, HEAD, POST,PUT, DELETE,TRACE, CONNECT, OPTIONS};
public:
	HttpParser();
	void setMsg(std::string msg);
	void parserLine(std::vector<std::vector<std::string> > &parsed);
	void parser(std::string msg, std::string key, std::vector<std::string> &tokens);
	void parser2(std::string msg, char key, std::vector<std::string> &tokens);

	~HttpParser();
};

class HttpMessage{
private :
	std::string request;
	std::string header;
	std::string content;
public :
	void setRequest(std::string method, std::string path);
	void setHeader(std::string host);
	void setContent(std::string content);
	std::string getMessage();
	std::string getCurrentTime();
	std::string getMetaData(std::string name, std::string value);
};

#endif
