/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:17:52 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/30 15:42:59 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Client::Client() : _fd(-1), _hasGivenPassword(false), _isRegistered(false),
					_nickname(""), _realname(""), _username("")
{
	std::cout << GREEN << "Default Client constructor called" << NC << std::endl;
}

Client::Client(int fd, struct sockaddr_in addr) : _fd(fd), _hasGivenPassword(false), _isRegistered(false) {
	(void)_fd;
	this->_ip_address = inet_ntoa(addr.sin_addr);
	std::cout << GREEN << "Client constructor with parameters called" << NC << std::endl;
}

// Setters
void	Client::setNickname(std::string const &nickname) {
	this->_nickname = nickname;
}

void Client::setUsername(std::string const &user) {
	this->_username = user;
}

void Client::setRealname(std::string const &real) {
	this->_realname = real;
}

void Client::setHasGivenPassword(bool value) {
	this->_hasGivenPassword = value;
}

void Client::setIsRegistered(bool value) {
	this->_isRegistered = value;
}

void	Client::setFd(int fd) {
	_fd = fd;
}

// Getters
std::string Client::getNickname() const {
	return this->_nickname;
}

std::string Client::getIPAdress() const {
	return this->_ip_address;
}

std::string Client::getUsername() const {
	return this->_username;
}

std::string Client::getRealname() const {
	return this->_realname;
}

bool Client::hasGivenPassword() const {
	return this->_hasGivenPassword;
}

bool Client::isRegistered() const {
	return this->_isRegistered;
}

int	Client::getFd() const {
	return this->_fd;
}