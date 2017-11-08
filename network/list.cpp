/*
 * list.cpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#include <thread>
#include <map>
#include "socket.hpp"
#include "list.hpp"

using namespace std;

namespace ps {

List::List(){
	headLink = NULL;
}

List::~List() {
	Link *tmp = headLink;
	Link *pre_tmp = NULL;

	while(tmp != NULL){
		pre_tmp = tmp;
		tmp = tmp->next;

		delete pre_tmp;
	}
}

bool List::empty(){
	if(headLink == NULL)
		return true;

	return false;
}

int List::size(){
	int size = 0;

	for(Link *tmp = headLink ; tmp!=NULL; tmp = tmp->next)
		size++;

	return size;
}

void List::insert(string data){
	Link *tmp = headLink;
	Link *pre_tmp = NULL;

	while(tmp != NULL){
		if(tmp->data == data)
			return;
		pre_tmp = tmp;
		tmp = tmp->next;
	}

	tmp = new Link;
	if(headLink != NULL)
		pre_tmp->next = tmp;
	else
		headLink = tmp;

	tmp->data = data;
	tmp->next = NULL;
}

bool List::search(string data){
	Link *tmp = headLink;

	while(tmp != NULL){
		if(tmp->data == data)
			return true;
		tmp = tmp->next;
	}

	return false;
}

void List::remove(string data){
	Link *tmp = headLink;
	Link *pre_tmp = NULL;

	while(tmp != NULL){
		if(tmp->data == data){

			if(headLink == tmp)
				headLink = tmp->next;
			else
				pre_tmp->next = tmp->next;

			delete tmp;

			break;
		}

		pre_tmp = tmp;
		tmp = tmp->next;
	}
}

string List::xport(){
	Link *tmp = headLink;
	string result = "";

	while(tmp != NULL){

		result += tmp->data;
		result += '\n';

		tmp = tmp->next;
	}

	return result;
}

void List::print(){
	cout << "--------------" << endl;
	cout << xport() <<endl;
	cout << "--------------" << endl;
}


} /* namespace ps */
