/*
 * HttpParser.cpp
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#include <iostream>
#include "Http.hpp"

HttpMessage::HttpMessage(string firstLine[], HeaderLine headerLine[], int headerSize, string content){
	for(int i=0;i<3;i++)
		this->firstLine[i] = firstLine[i];

	this->headerSize = headerSize;

	for(int i=0;i<headerSize;i++)
		this->headerLine[i] = headerLine[i];

	this->messageBody = content;
	error = true;
}

HttpMessage::HttpMessage(string msg){
	int start = 0, end = -1;
	int i = 0;
    this->headerSize = 0;
	while(1){
		end = msg.find("\r\n",start);
		string line = string(msg, start, end-start);
		if(i == 0){
			string first[512];
			if(parserLine(line, " ", first, 3)){
				this->firstLine[0] = first[0];
				this->firstLine[1] = first[1];
				this->firstLine[2] = first[2];
			}
			else{
				error= false;
				return ;
			}
		}
		else if(line != ""){
			string header[2];
			if(parserLine(line, ": ", header, 2)){
				this->headerLine[i-1].field = header[0];
				this->headerLine[i-1].value = header[1];
				this->headerSize = i;
			}
			else{
				error= false;
				return ;
			}
		}
		else{
			this->messageBody =string(msg, start+2);
			break;
		}

		start = end + string("\r\n").length();
		i++;
		if(end == -1)
			break;
	}
	error = true;
	return ;
}

HeaderLine* HttpMessage::getHeader(){
	return headerLine;
};
string HttpMessage::getMessageBody(){
	return messageBody;
}
int HttpMessage::getHeaderSize(){
	return headerSize;
}

bool HttpMessage::parserLine(string input, string key, string output[], int size){
	int s=0, e = -1;
	int i=0;
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
	return true;
}

string HttpMessage::getString(){
	string msg;
	msg = this->firstLine[0]+" "+this->firstLine[1]+" "+this->firstLine[2]+"\r\n";

	for(int i=0;i<this->headerSize;i++)
		msg += this->headerLine[i].field+": "+this->headerLine[i].value+"\r\n";

	msg += "\r\n";
	msg += this->messageBody;

	return msg;
}

HttpResponseMessge::HttpResponseMessge(string msg) : HttpMessage::HttpMessage(msg) {
	statusCode.version = this->firstLine[0];
	statusCode.status = this->firstLine[1];
	statusCode.message = this->firstLine[2];
}

HttpResponseMessge::HttpResponseMessge(StatusCode statusCode, HeaderLine headerLine[], int headerSize, string content)
: HttpMessage::HttpMessage(firstLine, headerLine, headerSize, content){
	string firstLine[3] = {statusCode.version, statusCode.status,statusCode.message};
	for(int i=0;i<3;i++)
		this->firstLine[i] = firstLine[i];
}

StatusCode HttpResponseMessge::getStatusCode(){
	return statusCode;
}

HttpRequestMessge::HttpRequestMessge(string msg) : HttpMessage::HttpMessage(msg) {
	requestLine.method = firstLine[0];
	requestLine.url = firstLine[1];
	requestLine.version = firstLine[2];
}

HttpRequestMessge::HttpRequestMessge(RequestLine requestLine, HeaderLine headerLine[], int headerSize, string content)
: HttpMessage::HttpMessage(firstLine, headerLine, headerSize, content){
	string firstLine[3] = {requestLine.method, requestLine.url, requestLine.version};
	for(int i=0;i<3;i++)
		this->firstLine[i] = firstLine[i];
}

RequestLine HttpRequestMessge::getRequestLine(){
	return requestLine;
}

