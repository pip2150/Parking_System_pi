/*
 * psAPI.cpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#include "psAPI.hpp"
#include "Http.hpp"

using namespace std;

void psapi::enter(ClientSocket& sock, string host,string plateNum){
	if(!sock.isValid())
		return;

	string buff = "";

	HttpMessage hm;
	hm.setRequest("POST", "/cars/" + plateNum + "/enter");
	hm.setHeader(host);
	buff = hm.getMessage();
	sock.send(buff);
}

void psapi::exit(ClientSocket& sock, string host, string plateNum){
	if(!sock.isValid())
		return;

	string buff = "";

	HttpMessage hm;
	hm.setRequest("POST", "/cars/" + plateNum + "/exit");
	hm.setHeader(host);
	buff = hm.getMessage();
	sock.send(buff);
}

void psapi::parking(ClientSocket& sock, string host, int floor, string zoneName, int zoneIndex, string plateNum){
	if(!sock.isValid())
		return;

	string buff = "";
	// 중괄호 넣을 것
	string jsondata =
			"{"
			"\"floor\": "+to_string(floor)+","
			"\"zone_name\": \""+zoneName+"\","
			"\"zone_index\": "+to_string(zoneIndex)+ ","
			"\"car_numbering\": \""+plateNum+"\""
			"}";

	HttpMessage hm;
	hm.setRequest("POST", "/places/update_state");
	hm.setContent(jsondata);
	hm.setHeader(host);
	buff = hm.getMessage();
	sock.send(buff);
}
