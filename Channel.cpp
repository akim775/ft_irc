/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:03:31 by ahamini           #+#    #+#             */
/*   Updated: 2026/01/05 14:02:50 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() : _name(""), _key("") {
	std::cout << GREEN << "Default Channel constructor called" << NC << std::endl;
}

Channel::Channel(std::string name, std::string key, Client *admin) : _name(name), _key(key) {
	std::cout << GREEN << "Channel constructor with parameters called" << NC << std::endl;
	this->addClient(admin);
	this->addOperator(admin);
}

Channel::~Channel() {
	_clients.clear();
	_operators.clear();
}

void	Channel::addClient(Client *newClient) {
	// Add client to the client list and check if he is already in the list
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]->getFd() == newClient->getFd())
			return;
	}
	_clients.push_back(newClient);
}

void	Channel::addOperator(Client *newOperator) {
	// Add client to the operator list and check if he is already in the list
	for (size_t i = 0; i < _operators.size(); i++) {
		if (_operators[i]->getFd() == newOperator->getFd())
			return;
	}
	_clients.push_back(newOperator);
}

void Channel::removeClient(int fd) {

	// Check if client is in the client list and remove it 
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getFd() == fd) {
			_clients.erase(it);
			break;
		}
	}
	//Check if client is in the operator list and remove it 
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if ((*it)->getFd() == fd) {
			_operators.erase(it);
			break;
		}
	}
}

void	Channel::broadcast(const std::string &message, int excludeFd) {
	for (size_t i = 0; i < _clients.size(); i++) {
		Client	*client = _clients[i];

		if (client->getFd() != excludeFd) {
			send(client->getFd(), message.c_str(), message.size(), 0);
		}
	}
}

bool Channel::isMember(Client *client) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]->getFd() == client->getFd())
			return true;
	}
	
	return false;
}

std::string	Channel::getKey() {
	return _key;
}

void	Channel::setKey(std::string key) {
	_key = key;
}