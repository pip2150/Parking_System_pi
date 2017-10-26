/*
 * http.cpp
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#include <cstring>
#include "http.hpp"

using namespace std;

http::Message::Message(std::string firstLine[], HeaderLine headerLine[], int headerSize, std::string content){

	for(int i=0;i<FIRSTLINESIZE;i++)
		this->firstLine[i] = firstLine[i];

	this->header = new HeaderLine[headerSize+1];
	this->headerSize = headerSize;

	for(int i=0 ;i < headerSize;i++)
		this->header[i] = headerLine[i];

	this->messageBody = content;

}

http::Message::Message(std::string msg){

	int start = 0, end = -1;
	int i = 0;
	this->headerSize = 0;

	this->header = new HeaderLine[MAXHEADERS];

	while(1){
		end = msg.find("\r\n",start);
		string line = string(msg, start, end-start);
		if(i == 0){
			string first[FIRSTLINESIZE];
			parser(line, " ", first, FIRSTLINESIZE);

			this->firstLine[0] = first[0];
			this->firstLine[1] = first[1];
			this->firstLine[2] = first[2];
		}
		else if(line != ""){
			string header[2];
			parser(line, ": ", header, 2);
			HeaderLine headerLine = { header[0], header[1] };

			this->header[i-1] = headerLine;
			this->headerSize = i;
		}
		else{
			this->messageBody =string(msg, start+2);
			break;
		}

		if(end == -1)
			break;

		start = end + string("\r\n").length();
		i++;

		if(i >= MAXHEADERS){
			std::cerr << __FUNCTION__ << " : "<< "Too many Headers in " << __FILE__ << " line : " << __LINE__ << std::endl;
			exit(1);
		}
	}

	return ;

}

http::Message::~Message(){

	delete header;
}

http::HeaderLine http::Message::getHeader(int index){

	return header[index];

};

std::string http::Message::getMessageBody(){

	return messageBody;

}

int http::Message::getHeaderSize(){

	return headerSize;

}

void http::Message::parser(std::string input, std::string key, std::string output[], int size){

	int s = 0, e = -1;
	int i = 0;
	while(1){
		e = input.find(key, s);

		if(i>=size)
			output[size-1] += key + string(input,s,e-s);
		else
			output[i] = string(input,s,e-s);

		s = e + key.length();
		i++;
		if(e == -1)
			break;
	}

}

std::string http::Message::getString(){

	string msg;
	msg = this->firstLine[0]+" "+this->firstLine[1]+" "+this->firstLine[2]+"\r\n";

	for(int i=0;i<this->headerSize;i++)
		msg += this->header[i].field+": "+this->header[i].value+"\r\n";

	msg += "\r\n";
	msg += this->messageBody;

	return msg;

}

http::ResponseMessge::ResponseMessge(std::string msg) : Message::Message(msg) {
}


http::ResponseMessge::ResponseMessge(StatusLine statusLine, HeaderLine headerLine[], int headerSize, std::string content)
: Message::Message(firstLine, headerLine, headerSize, content){

	string firstLine[FIRSTLINESIZE] = {statusLine.version, statusLine.status,statusLine.message};

	for(int i=0;i<FIRSTLINESIZE;i++)
		this->firstLine[i] = firstLine[i];

}

http::StatusLine http::ResponseMessge::getStatusLine(){

	StatusLine statusLine = {firstLine[0], firstLine[1], firstLine[2]};

	return statusLine;

}

http::RequestMessge::RequestMessge(std::string msg) : Message::Message(msg) {
}

http::RequestMessge::RequestMessge(RequestLine requestLine, HeaderLine headerLine[], int headerSize, std::string content)
: Message::Message(firstLine, headerLine, headerSize, content){

	string firstLine[FIRSTLINESIZE] = {requestLine.method, requestLine.url, requestLine.version};

	for(int i=0;i<FIRSTLINESIZE;i++)
		this->firstLine[i] = firstLine[i];

}

http::RequestLine http::RequestMessge::getRequestLine(){

	RequestLine requestLine ={firstLine[0], firstLine[1], firstLine[2]};

	return requestLine;

}
