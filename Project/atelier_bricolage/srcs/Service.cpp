/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Service.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wluong <wluong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 06:27:48 by wluong            #+#    #+#             */
/*   Updated: 2022/06/08 07:19:58 by wluong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Service.hpp"

/*
*	TO DO LIST :
*		Remplacer 512 par un define (nombre de clients max, soit 512 soit 1024)
*		Corriger le parsing de Victor pour pouvoir l'implémenter dansmes classes Server
*		Essayer des send et receive avec des un parsing existant
*		Clarifier le code au maximum (normalement deja bien calir)
*
*/

Service::Service() : _servers(), _max_sd(0) {
	_timeout.tv_sec = 5;
	_timeout.tv_usec = 0;
	setup_cluster();
}

/*=======================================
=				TO		DO				=
=======================================*/

Service::Service( Service const & src ) {}

Service::~Service() {}

Service & Service::operator=( Service const& other ) {}

/*=======================================
=					END					=
=======================================*/

void	Service::setup_cluster() {
	for (int i(0); i < 512; i++)
	{
		_clients_sd[i] = 0;
	}
}

void	Service::check_opened_sd() {

	int		socket_d;

	_max_sd = 0;
	FD_ZERO(_fdset);
	for (std::vector<Socket>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (*it->getSocket() > _max_sd)
			_max_sd = *it->getSocket();
		FD_SET(*it->getSocket(), _fdset);
	}
	for (int i(0); i < 512; i++)
	{
		socket_d = _clients_sd[i];
		if (socket_d > 0)
			FD_SET(socket_d, _fdset)
		if (socket_d > _max_sd)
			_max_sd = socket_d;
	}
}

void	Service::run_service() {
	while (TRUE)
	{
		this->check_opened_sd();
		this->selecting();
		this->accepting_connections();
		this->receive();
	}
}

bool	Service::selecting() {
	
	int		activity = 0;

	activity = select( _max_sd + 1, &_fdset, NULL, NULL, &_timeout);
	if (activity < 0)
	{
		std::cout << _BL_RED << "ERROR : " << _NOR << "SELECT ERROR" << std::endl;
		return false;
	}
	return true;

}

bool	Service::accepting_connections() {

	int		new_connection;

	for (std::vector<Socket>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (FD_ISSET(*it->getSocket(), &_fdset))
		{
			new_connection = accept(*it->getSocket(), *it->castAddr(), *it->getAdLen());
			if (new_connection < 0)
			{
				std::cout << _BL_RED << "ERROR : " << _NOR << "ACCEPT ERROR" << std::endl;
				return false;
			}
			std::cout << _GR << "NEW CONNECTION" << _NOR << ", SOCKET FD IS " << new_socket << ", PORT IS: " << ntohs(master_socket.getAddr().sin_port) << std::endl;
		}
		for (int i(0); i < 512; i++)
		{
			if (_clients_sd[i]== 0)
			{
				_clients_sd[i] = new_connection;
				std::cout << "Socket added at position " << i << std::endl;
				break ;
			}
		}
	}
	return true;
}

void	Service::receive() {

	int		len_recv;

	for (int i(0); i < 512; i++)
	{
		if(FD_ISSET(_clients_sd[i], &_fdset))
		{
			len_recv = recv(_clients_sd[i], _buffer, 1024, 0);
			if (len_recv < 0)
			{
				std::cout << _BL_RED << "ERROR : " << _NOR << "RECV ERROR" << std::endl;
				continue;
			}
			if (len_recv == 0 || (len_recv == 1 && _buffer[0] = 4))
			{
				close(_clients_sd[i]);
				_clients_sd[i] = 0;
			}
			_buffer[len_recv] = 0;
		}
		sending(i);
	}
}

bool	Service::sending(int i) {
	std::string header = "HTTP/1.1 200 OK\nContent-Type: text/html\n";
	std::ifstream is("../../www/index.html");
	std::string line; //c++ buffer style
	if (is.is_open()) {
		while (std::getline(is, line)) { //get a line
			header += line + '\n';
		}
	}
	is.close();
	send(new_socket, header.c_str(), header.length(), 0);
}

std::vector<Socket>		Service::getServers() const {
	return _servers;
}