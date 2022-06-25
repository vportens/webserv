/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wluong <wluong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 18:56:51 by wluong            #+#    #+#             */
/*   Updated: 2022/06/25 04:59:38 by wluong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

char *itoa(int num, char* buffer, int base) {

	int curr = 0;

	if (num == 0) {
		buffer[curr++] = '0';
		buffer[curr] = '\0';
		return buffer;
	}

	int num_digits = 0;

	if (num < 0) {
		if (base == 10) {
			num_digits ++;
			buffer[curr] = '-';
			curr ++;
			num *= -1;
		}
		else
			return NULL;
	}

	num_digits += (int)floor(log(num) / log(base)) + 1;
 
	while (curr < num_digits) {
		int base_val = (int) pow(base, num_digits-1-curr);

		int num_val = num / base_val;
 
		char value = num_val + '0';
		buffer[curr] = value;
 
		curr ++;
		num -= base_val * num_val;
	}
	buffer[curr] = '\0';
	return buffer;
}

std::string		int_to_str(size_t size){
	char *intStr = NULL;
	itoa(size, intStr, 10);
	std::string	ret(intStr);
	return ret;
}


Response::Response(request req) : _header(), _body(), _request(req), _resp() {}

Response::~Response() {}

std::string		Response::getHeader() {
	return _header.getHeader();
}

std::string		Response::getBody() {
	return _body;
}

std::string		Response::getResponse() {
	_header.generateHeader();
	_resp = _header.getHeader();
	_resp += _body;
	return _resp;
}

// bool			Response::AllowedMethod() {
// 	// if (this->_request.getMethod() == "GET")
// 	// 	return true;
// }

void			Response::responseGet() {
	/* NECESSITE LE PQRSING DE VICTOR
	du coup en attendant */
	// bool location = 1;
	// bool autoindex = 1;

	if (this->_request.get_path() == "/")
	{
		// if (autoindex)
			// responseAutoIndex();
		//check if error page exists
		// this->_body = setErrorPage();
		//else
		//generate error page
	}
	this->_header.setStatus("200 ", "OK");
	//fill the header
	this->_body = readFromFile("../../www/index.html");
	this->_header.setContentLength(int_to_str(_body.size()));
	this->_header.setServerName("webserv");

}

void			Response::responsePost() {
	
}

void			Response::responseDelete() {
	
	//is method allowed
	
	if (std::remove(this->_request.get_path().c_str()))
		this->_header.setStatus("403 ", "Forbidden");
	//creer un body pour dire que c delete ?
}

void			Response::responseCGI() {

}

void			Response::responseAutoIndex() {

}

std::string		Response::readFromFile(std::string path) {
	std::ifstream		ifs(path.c_str());
	std::string			ret_buffer;
	std::string			tmp;

	if (!ifs)
	{
		std::cout << "This file doesn't exist." << std::endl;
		return ("");
	}
	while (!ifs.eof())
	{
		std::getline(ifs, tmp);
		ret_buffer += tmp;
		ret_buffer += "\n";
	}
	ifs.close();
	return ret_buffer;
}
