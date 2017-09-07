/*
 * psAPI.cpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

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

ps::API::API(){
	hostname = "13.124.74.249";
	port = 3000;

	if(!sock.connect(hostname, port))
		::exit(1);

};

void ps::API::setHeader(HeaderLine headerLine[], string content){
	HeaderLine host = { "Host", hostname };
	HeaderLine connection = {"Connection", "keep-alive"};
	HeaderLine date = {"Date", getCurrentTime() };
	HeaderLine contentType = {"Content-Type","application/json; charset=UTF-8"};
	HeaderLine contentEncoding = {"Content-Encoding","UTF-8"};
	HeaderLine server = {"Server","PSystem (Unix) (Raspbian/Linux)"};
	HeaderLine acceptRange = {"Accept-Ranges","bytes"};
	HeaderLine contentLength = {"Content-Length", to_string(content.length()) };
	HeaderLine headers[] = {host, connection, date, contentType, contentEncoding, server , acceptRange, contentLength};

	for(int i = 0; i< HEADERNUM; i++ )
		headerLine[i] = headers[i];
}

void ps::API::inout(string plateNum, string inout){
	RequestLine requestLine = {"POST", "/cars/" + plateNum + inout, "HTTP/1.1"};
	int headerSize = HEADERNUM;
	HeaderLine headerLine[headerSize];
	setHeader(headerLine,"");
	HttpRequestMessge http(requestLine, headerLine, headerSize, "");
	string buff = http.getString();

	if(!sock.send(buff))
		::exit(1);
}

void ps::API::parking(int floor, string zoneName, int zoneIndex, string plateNum){
	RequestLine requestLine = {"POST", "/places/update_state", "HTTP/1.1"};
	int headerSize = HEADERNUM;
	HeaderLine headerLine[headerSize];
	string jsondata =
			"{"
			"\"floor\": "+to_string(floor)+","
			"\"zone_name\": \""+zoneName+"\","
			"\"zone_index\": "+to_string(zoneIndex)+ ","
			"\"car_numbering\": \""+plateNum+"\""
			"}";
	setHeader(headerLine,jsondata);
	HttpRequestMessge http(requestLine, headerLine, headerSize, jsondata);
	string buff = http.getString();

	if(!sock.send(buff))
		::exit(1);
}

void ps::API::exit(string plateNum){
	ps::API::inout(plateNum,"/exit");
}

void ps::API::enter(string plateNum){
	ps::API::inout(plateNum,"/enter");
}

void ps::API::resopnse(){
	string recvbuff="";
	string buff="";
	string content = "";

	int size = MAXRECV;
	while(size >= MAXRECV){
		size = sock.recv(buff);
		recvbuff += buff; 
        if(size == -1)
            break;
	}

	HttpRequestMessge http(recvbuff);

	cout << http.getRequestLine().url << " ";
	cout << http.getRequestLine().version << endl;

	content += http.getMessageBody();

	int contentLength = 0;
	int headerSize = http.getHeaderSize();

	for(int i=0;i<headerSize; i++){
		HeaderLine header = http.getHeader()[i];
		if(!header.field.compare("Content-Length")){
			contentLength = atoi(header.value.c_str());
			break;
		}
	}

	contentLength -= http.getMessageBody().length();

	while(contentLength > 0){
		contentLength -= sock.recv(buff);
		content += buff;
	}

	FILE *html = fopen("output.html", "w+");
	fwrite(content.c_str(), content.length(), 1, html);
	fclose(html);

}
