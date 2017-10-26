/*
 * psAPI.cpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#include <cstring>
#include "socket.hpp"
#include "http.hpp"
#include "psAPI.hpp"

using namespace std;

string getCurrentTime(){

	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char *time = asctime(timeinfo);
	time[strlen(time)-1]='\0';
	return string(time);

}

ps::API::API(std::string hostname, int port){

	this->hostname = hostname;
	this->port = port;
	sock = new sock::ClientSocket();

	if(!sock->connect(hostname, port))
		::exit(1);

};
ps::API::~API(){

    cout << " ---- " <<endl;
    delete sock;
    cout << " ---- " <<endl;
}

void ps::API::setHeader(http::HeaderLine headerLine[], string content){

	http::HeaderLine host = { "Host", hostname };
	http::HeaderLine connection = {"Connection", "keep-alive"};
	http::HeaderLine date = {"Date", getCurrentTime() };
	http::HeaderLine contentType = {"Content-Type","application/json; charset=UTF-8"};
	http::HeaderLine contentEncoding = {"Content-Encoding","UTF-8"};
	http::HeaderLine server = {"Server","PSystem (Unix) (Raspbian/Linux)"};
	http::HeaderLine acceptRange = {"Accept-Ranges","bytes"};
	http::HeaderLine contentLength = {"Content-Length", to_string(content.length()) };
	http::HeaderLine headers[] = {host, connection, date, contentType, contentEncoding, server , acceptRange, contentLength};

	for(int i = 0; i< HEADERSIZE; i++ )
		headerLine[i] = headers[i];

}

void ps::API::inout(string plateStr, string inout){

	http::RequestLine requestLine = {"POST", "/cars/" + plateStr + inout, "HTTP/1.1"};

	http::HeaderLine headerLine[HEADERSIZE];
	setHeader(headerLine,"");

	http::RequestMessge http(requestLine, headerLine, HEADERSIZE, "");
	string buff = http.getString();

	if(!sock->send(buff))
		::exit(1);

}

void ps::API::parking(int floor, string zoneName, int zoneIndex, string plateStr){

	http::RequestLine requestLine = {"POST", "/places/update_state", "HTTP/1.1"};

	http::HeaderLine headerLine[HEADERSIZE];
	string jsondata =
			"{"
			"\"floor\": "+to_string(floor)+","
			"\"zone_name\": \""+zoneName+"\","
			"\"zone_index\": "+to_string(zoneIndex)+ ","
			"\"car_numbering\": \""+plateStr+"\""
			"}";
	setHeader(headerLine,jsondata);

	http::RequestMessge http(requestLine, headerLine, HEADERSIZE, jsondata);
	string buff = http.getString();

	if(!sock->send(buff))
		::exit(1);

}

void ps::API::exit(string plateStr){

	ps::API::inout(plateStr,"/exit");

}

void ps::API::enter(string plateStr){

	ps::API::inout(plateStr,"/enter");

}

bool ps::API::resopnse(){

	const string END_OF_HTTP = "\r\n\r\n";		// CR LF CR LF
	string recvbuff="";
	string buff="";

	while(1){
		int size = sock->recv(&buff);
		recvbuff += buff;

		if(size == -1)
			return false;

		auto end = buff.end();
		if(string(end-4, end) == END_OF_HTTP)
			break;
	}

	http::ResponseMessge http(recvbuff);

	http::StatusLine statusLine = http.getStatusLine();
	cout << statusLine.message << " ";
	cout << statusLine.status << endl;

	string content = http.getMessageBody();

	FILE *html = fopen("resopnse.html", "w+");

	if(html == NULL){
		perror("fopen");
		return false;
	}

	fwrite(content.c_str(), content.length(), 1, html);
	fclose(html);

	return true;

}
