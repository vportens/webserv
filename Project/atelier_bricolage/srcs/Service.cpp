/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Service.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wluong <wluong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 06:27:48 by wluong            #+#    #+#             */
/* ************************************************************************** */

#include "../includes/Service.hpp"

Service::Service() : _max_sd(0), _servers() {
	_timeout.tv_sec = 5;
	_timeout.tv_usec = 0;
	setup_cluster();
}

/*=======================================
=				TO		DO				=
=======================================*/

// Service::Service( Service const & src ) {}

Service::~Service() {}

// Service & Service::operator=( Service const& other ) {}

/*=======================================
=					END					=
=======================================*/

void	Service::setup_cluster() {
	for (int i(0); i < MAX_CLIENTS; i++)
	{
		_clients_sd[i] = 0;
	}
}

void	Service::check_opened_sd() {

	int		socket_d;

	_max_sd = 0;
	FD_ZERO(&_fdset);
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->_serv_sock.getSocket() > _max_sd)
			_max_sd = it->_serv_sock.getSocket();
		FD_SET(it->_serv_sock.getSocket(), &_fdset);
	}
	for (int i(0); i < MAX_CLIENTS; i++)
	{
		socket_d = _clients_sd[i];
		if (socket_d > 0)
			FD_SET(socket_d, &_fdset);
		if (socket_d > _max_sd)
			_max_sd = socket_d;
	}
}

void	Service::run_service() {
	while (TRUE)
	{
		this->check_opened_sd();
		if (!this->selecting())
			exit(EXIT_FAILURE);
		if (!this->accepting_connections())
			exit(EXIT_FAILURE);
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

	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (FD_ISSET(it->_serv_sock.getSocket(), &_fdset))
		{
			new_connection = accept(it->_serv_sock.getSocket(), it->_serv_sock.castAddr(), it->_serv_sock.getAdLen());
			if (new_connection < 0)
			{
				std::cout << _BL_RED << "ERROR : " << _NOR << "ACCEPT ERROR" << std::endl;
				return false;
			}
			fcntl(new_connection, F_SETFL, O_NONBLOCK);
			std::cout << _GRE << "NEW CONNECTION" << _NOR << ", SOCKET FD IS " << new_connection << ", PORT IS: " << ntohs(it->_serv_sock.getAddr().sin_port) << std::endl;
		}
		for (int i(0); i < MAX_CLIENTS; i++)
		{
			if (_clients_sd[i]== 0)
			{
				_clients_sd[i] = new_connection;
				break ;
			}
		}
	}
	return true;
}

void	Service::receive() {

	int		len_recv;

	for (int i(0); i < MAX_CLIENTS; i++)
	{	
		if(FD_ISSET(_clients_sd[i], &_fdset))
		{
			len_recv = recv(_clients_sd[i], _buffer, 10024, 0);
			if (len_recv < 0)
			{
				std::cout << _BL_RED << "ERROR : " << _NOR << "RECV ERROR" << std::endl;
				break;
			}
			if (len_recv == 0 || (len_recv == 1 && _buffer[0] == 4))
			{
				close(_clients_sd[i]);
				_clients_sd[i] = 0;
			}
			_buffer[len_recv] = 0;
			std::cout << _buffer << std::endl;
			
			//do while

			request req(_buffer, _servers);
			std::cout << "request traiter" <<std::endl;
			// _buffer[0] = 0;
//			if (req)
			// {
			Response	resp(req);
			resp.responseGet(_servers);
			std::cout << resp.getResponse() << std::endl;
			sending(i, resp);
			// }

			/* IF BAD REQUEST
			create a bad request response .html
			sending this html
			close client_sd[i]
			client_sd[i] = 0 */
			close(_clients_sd[i]);
			_clients_sd[i] = 0;
		}
		// parsing request sur _buffer 
		// sending doit recevoir la stc du parsing request

		// METHOD TREATMENT: after parsing
		// GET : CGI ? -> Create CGI 
		// check ressource at / 
		// 
	}
}

void	Service::sending(int i, Response resp) {
	send(_clients_sd[i], resp.getResponse().c_str(), resp.getResponse().length(), 0);
}

// std::vector<Socket>		&Service::getServers() const {
// 	return _servers;
// }
