/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:03:28 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/31 10:12:04 by ahamini          ###   ########.fr       */
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
	std::vector<Client> _clients;
	std::vector<Client> _operators;

public:
	Channel();
	Channel(std::string name, std::string key, Client *admin);
};

#endif