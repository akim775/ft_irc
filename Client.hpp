/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:17:54 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/23 15:07:54 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client {
	private:
		int	_fd;
		std::string	_ip_address;
		std::string	_nickname;
		std::string	_username;
		std::string	_buffer;

	public:
		Client() : _fd(-1) {};
		Client(int fd, struct sockaddr_in addr) : _fd(fd) {
			(void)_fd;
			this->_ip_address = inet_ntoa(addr.sin_addr);
		};
		std::string & getBuffer() { return _buffer; }
};

#endif