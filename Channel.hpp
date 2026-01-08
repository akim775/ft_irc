/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilsadi <ilsadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:03:28 by ahamini           #+#    #+#             */
/*   Updated: 2026/01/08 17:06:22 by ilsadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <set>

class Channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::set<std::string> _invited;
		std::vector<Client *> _clients;
		std::vector<Client *> _operators;

	public:
		Channel();
		Channel(std::string name, std::string key, Client *admin);
		~Channel();

		// Getters
		std::string	getName();
		std::string	getKey();
		std::vector<Client *>	getClients() const;
		size_t getClientCount() const;
		std::string getTopic() const;
		// Setters
		void	setKey(std::string key);
		void	setTopic(const std::string &topic);

		void	addClient(Client *newClient);
		void	addOperator(Client *newAdmin);
		void	removeClient(int fd);
		void	broadcast(const std::string &message, int excludeFd = -1);
		bool	isMember(Client *client);
		bool	isOperator(Client *client) const;
		void	inviteNick(const std::string &nickname);
		bool	isInvited(const std::string &nickname) const;
		void	revokeInvite(const std::string &nickname);
	};
#endif