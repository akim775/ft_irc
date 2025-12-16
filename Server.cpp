/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 12:01:18 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/16 18:23:16 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

volatile bool g_signal = true;

Server::Server(int port, std::string &password) : _port(port), _password(password), _server_fdsocket(-1), _epoll_fd(-1) {
	std::cout << BLUE << "Server constructor with parameters called" << NC << std::endl;
	(void)_port;
	(void)_epoll_fd;
}

Server::~Server() {
	if (_server_fdsocket != -1)
		close(_server_fdsocket);
	if (_epoll_fd != -1)
		close(_epoll_fd);
	std::cout << BLUE << "Server destructor called" << NC << std::endl;
}

void	Server::initialize_socket() {

	struct sockaddr_in address_serv;
	std::memset(&address_serv, 0, sizeof(address_serv));
	address_serv.sin_family = AF_INET;
	address_serv.sin_addr.s_addr = INADDR_ANY;
	address_serv.sin_port = htons(_port);
	int	optval = 1;
	
	_server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fdsocket == -1)
		throw std::runtime_error(std::string("socket() function failed : ") + strerror(errno));
	if (setsockopt(_server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw std::runtime_error(std::string("setsockopt() function failed : ") + strerror(errno));
	if (fcntl(_server_fdsocket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl() function failed to put O_NONBLOCK option: ") + strerror(errno));
	if (bind(_server_fdsocket, (struct sockaddr *)&address_serv, sizeof(address_serv)) == -1)
		throw std::runtime_error(std::string("bind() function failed : ") + strerror(errno));
	if (listen(_server_fdsocket, SOMAXCONN) == -1)
		throw std::runtime_error(std::string("listen() function failed : ") + strerror(errno));
}

void	Server::setup_epoll() {

	_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (_epoll_fd == -1)
		throw std::runtime_error(std::string("epoll_create1() function failed : ") + strerror(errno));
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _server_fdsocket;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fdsocket, &event) == -1)
		throw std::runtime_error(std::string("epoll_ctl() function failed for _server_fdsocket : ") + strerror(errno));
}

void	Server::start() {
	struct epoll_event events[MAX_EVENTS];
	while (g_signal) {
		int nbr_fd = epoll_wait(_epoll_fd, events, MAX_EVENTS, 500);
		if (nbr_fd == -1) {
			if (errno == EINTR)
				continue;
			throw std::runtime_error(std::string("epoll_wait failed : ") + strerror(errno));
		}
		for (int i = 0; i < nbr_fd; ++i) {
			if (!g_signal)
				break;
			if (events[i].data.fd == _server_fdsocket)
				accept_new_client();
			else
				handle_client_data();
		}
	}
}

void	Server::accept_new_client() {
	struct sockaddr_in address_client;
	socklen_t client_len = sizeof(address_client);
	while (true) {
		std::memset(&address_client, 0, sizeof(address_client));
		int client_fd = accept(_server_fdsocket, (struct sockaddr *)&address_client, &client_len);

		if (client_fd == - 1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			std::cerr << RED << "Error : accept() function failed : " << strerror(errno) << std::endl;
			break;
		}
		if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
			std::cerr << RED << "Error: fcntl() function failed for client fd " << client_fd << NC << std::endl;
			close(client_fd);
			continue;;
		}
	}
}

void	Server::handle_client_data() {
	
}