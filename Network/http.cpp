/*
 * HttpParser.cpp
 *
 *  Created on: 2017. 7. 28.
 *      Author: dhrco
 */

#include "Http.hpp"


using namespace std;

HttpParser::HttpParser(){
	msg = "";
}
void HttpParser::setMsg(string msg){
	this->msg = msg;
}

void HttpParser::parserLine(vector<vector<string> > &parsed){
	vector<string> tokens;
	parser(msg, "\r\n", tokens);

	for(unsigned int i=0;i<tokens.size();i++){
		vector<string> parse;
		parser2(tokens[i], ':', parse);
		parsed.push_back(parse);
	}
}

void HttpParser::parser(string msg, string key, vector<string> &tokens){
	tokens.clear();
	char *msg_;

	msg_ = new char(msg.length()+1);
	strcpy(msg_,msg.c_str());

	char *token = strtok(msg_, key.c_str());

	int i=0;
	while (token != NULL) {
		cout << "LINE"<<__LINE__ << ":" << i <<endl;
		tokens.push_back(string(token));
		token = strtok(NULL, key.c_str());
		i++;
	}

	delete(msg_);
}
void HttpParser::parser2(string msg, char key, vector<string> &tokens){
	string token = "";

	bool cnt = false;
	for(size_t i=0;i<msg.length();i++){
		if(msg[i] == key){
			if(!cnt){
				tokens.push_back(token);
				token = "";
				cnt = true;
				if(msg[i+1] == ' ')
					i++;
				continue;
			}
		}
		token += msg[i];
	}
	tokens.push_back(token);

}

HttpParser::~HttpParser(){

}
HttpMessage::HttpMessage(){
	request = "";
	header = "";
	content = "";
}

void HttpMessage::setRequest(string method, string path){
	request =  method+ " " +path+ " HTTP/1.1" + "\r\n";
}

void HttpMessage::setHeader(string host){
	header =
			getMetaData("Host", host) +
			getMetaData("Connection", "keep-alive") +
			getMetaData("Date",getCurrentTime()) +
			getMetaData("Content-Type","application/json; charset=UTF-8") +
			getMetaData("Content-Encoding","UTF-8") +
			getMetaData("Server","PSystem (Unix) (Raspbian/Linux)") +
			getMetaData("Accept-Ranges","bytes") +
			getMetaData("Content-Length",to_string(content.length()));
}

void HttpMessage::setContent(string content){
	this->content = content;
}

string HttpMessage::getMessage(){
	return request + header + "\r\n" + content;
}

string HttpMessage::getCurrentTime(){
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char *time = asctime(timeinfo);
	time[strlen(time)-1]='\0';
	return string(time);
}

string HttpMessage::getMetaData(string name, string value){
	return name + ": " + value+"\r\n";
}
