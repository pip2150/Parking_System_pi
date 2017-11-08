/*
 * list.hpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: dhrco
 */

#ifndef LIST_HPP_
#define LIST_HPP_

#include <iostream>

namespace ps {

class List {

	struct Link{
		std::string data;
		Link *next;
	};

	Link *headLink;

public:

	List();

	virtual ~List();

	bool empty();

	int size();

	void insert(std::string data);

	bool search(std::string data);

	void remove(std::string data);

	std::string xport();

	void print();

};


} /* namespace ps */

#endif /* LIST_HPP_ */
