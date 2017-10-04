/*
 * HttpParser.h
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#ifndef HTTP_HPP_
#define HTTP_HPP_

using namespace std;

struct HeaderLine {
	string field;
	string value;
};

struct StatusCode{
	string version;
	string status;
	string message;
};

struct RequestLine{
	string method;
	string url;
	string version;
};

class HttpMessage {
protected:
	string firstLine[3];
private :
	HeaderLine headerLine[500];
	string messageBody;
	bool error;
	int headerSize;

	bool parserLine(string input, string key, string output[], int size);

public:
	HttpMessage(string msg);
	HttpMessage(string firstLine[], HeaderLine headerLine[], int headerSize, string content);

	HeaderLine* getHeader();
	string getMessageBody();
	int getHeaderSize();
	string getString();
};

class HttpResponseMessge : public HttpMessage{
private:
	StatusCode statusCode;

public:
	HttpResponseMessge(string msg);
	HttpResponseMessge(StatusCode statusCode, HeaderLine headerLine[], int headerSize, string content);

	StatusCode getStatusCode();
};

class HttpRequestMessge : public HttpMessage{
private:
	RequestLine requestLine;

public :
	HttpRequestMessge(string msg);
	HttpRequestMessge(RequestLine requestLine, HeaderLine headerLine[], int headerSize, string content);

	RequestLine getRequestLine();
};

#endif
