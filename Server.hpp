/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 14:53:30 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/25 16:27:13 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Client.hpp"
#include "colors.hpp"

#define MAX_EVENTS 128
#define BUFFER_SIZE 4096
#define NOT_ENOUGH_PARAM 461
#define ALREADY_REGISTERED 462
#define BAD_PASSWORD 464



class Server {
	private:
		int	_port;
		std::string	_password;
		int	_server_fdsocket;
		int	_epoll_fd;
		std::map<int, Client> _clients;
		void	cmd_parsing(int fd, const std::string &command_line);
		void	cmd_pass(int fd, const std::vector<std::string> &args);
		std::map<std::string, void (Server::*)(int, const std::vector<std::string>&)> _cmds;
		std::vector<std::string> Server::splitMessage(std::string input);


	public:
		Server(int port, std::string &password);
		~Server();

		void	initialize_socket();
		void	setup_epoll();
		void	add_fd_to_epoll(int fd);
		void	start();
		private:
		void	accept_new_client();
		void	handle_client_data(int fd);
		void	onClientDisconnect(int fd);

	};

extern volatile bool g_signal;

#endif