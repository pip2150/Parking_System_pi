/*
 * psAPI.cpp
 *
 *  Created on: 2017. 8. 7.
 *      Author: dhrco
 */

#include "psAPI.hpp"
#include "Http.hpp"

using namespace std;

void psapi::carEnter(ClientSocket& sock, string host,string plateNum){
	if(!sock.isValid())
		return;

	string buff = "";
	string jsondata = "{\"id\":\""+plateNum+"\"}";

	HttpMessage hm;
	hm.setRequest("POST", "/cars/" + plateNum + "/enter");
	hm.setContent(jsondata);
	hm.setHeader(host);
	buff = hm.getMessage();
	sock.send(buff);
}

void psapi::carExit(ClientSocket& sock, string host, string plateNum){
	if(!sock.isValid())
		return;

	string buff = "";
	string jsondata = "{\"id\":\""+plateNum+"\"}";

	HttpMessage hm;
	hm.setRequest("POST", "/cars/" + plateNum + "/exit");
	hm.setContent(jsondata);
	hm.setHeader(host);
	buff = hm.getMessage();
	sock.send(buff);
}

void psapi::carParking(ClientSocket& sock, string host, int floor, string zoneName, int zoneIndex, string plateNum){
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
