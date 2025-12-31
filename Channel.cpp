/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:03:31 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/31 10:12:18 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() : _name(""), _key("") {
	std::cout << GREEN << "Default Channel constructor called" << NC << std::endl;
}

Channel::Channel(std::string name, std::string key, Client *admin) : _name(name), _key(key) {
	std::cout << GREEN << "Channel constructor with parameters called" << NC << std::endl;
}