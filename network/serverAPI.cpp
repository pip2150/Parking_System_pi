/*
 * psAPI.cpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#include <cstring>
#include "socket.hpp"
#include "http.hpp"
#include "serverAPI.hpp"

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

ps::ServerAPI::ServerAPI(std::string hostname, int port) : API(hostname, port){

};

void ps::ServerAPI::setHeader(http::HeaderLine headerLine[], string content){

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

void ps::ServerAPI::inout(string plateStr, string inout) {

	http::RequestLine requestLine = {"POST", "/cars/" + plateStr + inout, "HTTP/1.1"};

	http::HeaderLine headerLine[HEADERSIZE];
	setHeader(headerLine,"");

	http::RequestMessge http(requestLine, headerLine, HEADERSIZE, "");
	string buff = http.getString();

	if(!sock->send(buff))
		::exit(1);

}

void ps::ServerAPI::parking(int floor, string zoneName, int zoneIndex, string plateStr) {

	http::RequestLine requestLine = {"POST", "/places/update_state", "HTTP/1.1"};

	http::HeaderLine headerLine[HEADERSIZE];
	string jsondata =
			"{"
			"\"floor\": "+to_string(floor)+","
			"\"zone_name\": \""+zoneName+"\","
			"\"zone_index\": "+to_string(zoneIndex)+ ","
            ;
    if (plateStr == "") {
        jsondata +=	
            "\"car_numbering\": null"
            "}";
    }
    else {
        jsondata +=
			"\"car_numbering\": \""+plateStr+"\""
			"}";
    }

	setHeader(headerLine,jsondata);

	http::RequestMessge http(requestLine, headerLine, HEADERSIZE, jsondata);
	string buff = http.getString();

	if(!sock->send(buff))
		::exit(1);

}

void ps::ServerAPI::exit(string plateStr) {

	inout(plateStr,"/exit");

}

void ps::ServerAPI::enter(string plateStr) {

	inout(plateStr,"/enter");

}

void ps::ServerAPI::loadAll(std::vector<std::string> &texts) {

	http::RequestLine requestLine = {"GET", "/cars/all_entered_car_numberings", "HTTP/1.1"};

	http::HeaderLine headerLine[HEADERSIZE];
	setHeader(headerLine,"");

	http::RequestMessge rm(requestLine, headerLine, HEADERSIZE, "");
	string buff = rm.getString();

	if(!sock->send(buff))
		::exit(1);

	const string END_OF_HTTP = "\r\n\r\n";		// CR LF CR LF
	string recvbuff="";
	buff="";

	while(1){
		int size = sock->recv(&buff);
		recvbuff += buff;
		if(size == -1)
            break;

		auto end = buff.end();
        if(buff.find(END_OF_HTTP) != -1)
			break;
	}

	http::ResponseMessge http(recvbuff);

	http::StatusLine statusLine = http.getStatusLine();
	cout << statusLine.message << " ";
	cout << statusLine.status << endl;

	string content = http.getMessageBody();

    int start = (int)content.find('[');
    int end = (int)content.find(']');
    
    string numberings = string(content, start+1,end-start-1);
    
    if(numberings.length() == 0)
        return;

    int s = 0, e = -1;
    int i = 0;

    while(1){
        e = numberings.find(",",s);
        string text;

        if(e == -1)
            text = string(numberings, s+1, numberings.length()-s-2);
        else
            text = string(numberings, s+1, e-s-2);

        texts.push_back(text);

        s = e + 1;
        i++;
        if(e == -1)
            break;
    }

}

bool ps::ServerAPI::resopnse() {

	const string END_OF_HTTP = "\r\n\r\n";		// CR LF CR LF
	string recvbuff="";
	string buff="";

	while(1){
		int size = sock->recv(&buff);
		recvbuff += buff;
		if(size == -1)
			return false;

		auto end = buff.end();
        if(buff.find(END_OF_HTTP) != -1)
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
