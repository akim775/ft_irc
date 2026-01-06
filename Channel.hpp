/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:03:28 by ahamini           #+#    #+#             */
/*   Updated: 2026/01/06 01:03:45 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::string _key;
	std::vector<Client *> _clients;
	std::vector<Client *> _operators;

public:
	Channel();
	Channel(std::string name, std::string key, Client *admin);
	~Channel();

	// Getters
	std::string	getName();
	std::string	getKey();
	std::string	getTopic() const;
    std::vector<Client *>	getClients() const;
	
	// Setters
	void	setKey(std::string key);
	
	void	addClient(Client *newClient);
	void	addOperator(Client *newAdmin);
	void	removeClient(int fd);
	void	broadcast(const std::string &message, int excludeFd = -1);
	bool	isMember(Client *client);
	bool	isOperator(Client *client) const;
};

#endif