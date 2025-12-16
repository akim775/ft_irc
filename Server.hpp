/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 14:53:30 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/10 14:55:09 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP

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
#include "colors.hpp"

#define MAX_EVENTS 128

class Server {
	private:
		int	_port;
		std::string	_password;
		int	_server_fdsocket;
		int	_epoll_fd;

	public:
		Server(int port, std::string &password);
		~Server();

		void	initialize_socket();
		void	setup_epoll();
		void	start();
	private:
		void	accept_new_client();
		void	handle_client_data();
};

extern volatile bool g_signal;

#endif