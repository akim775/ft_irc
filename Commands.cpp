/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 15:46:43 by ahamini           #+#    #+#             */
/*   Updated: 2025/12/25 15:51:57 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

std::vector<std::string> Server::splitMessage(std::string input) {
	std::vector<std::string> args;
	size_t pos = 0;

	while (pos < input.length()) {
		// Skip spaces
		while (pos < input.length() && input[pos] == ' ') {
			pos++;
		}
		
		// Check if we are at the end of the string
		if (pos >= input.length()) {
			break;
		}

		// Push everything after the ':' sign 
		if (input[pos] == ':') {
			args.push_back(input.substr(pos + 1));
			// For IRC, ':' is the last args
			return args; 
		}

		// Find the next space
		size_t next_space = input.find(' ', pos);

		if (next_space == std::string::npos) {
			// If no more space, it's the last word
			args.push_back(input.substr(pos));
			break;
		} else {
			// On extrait le mot entre la position actuelle et le prochain espace
			args.push_back(input.substr(pos, next_space - pos));
			// On avance notre curseur sur l'espace trouvé
			pos = next_space;
		}
	}
	return args;
}

void Server::cmd_parsing(int fd, const std::string &cmd_line) {
	if (cmd_line.empty())
		return;
	std::vector<std::string> args = splitMessage(cmd_line);
	if (args.empty())
		return;

	// Put the command in uppercase (ex: "nick" -> "NICK")
	std::string cmdName = args[0];
	for (size_t i = 0; i < cmdName.length(); i++)
		cmdName[i] = std::toupper(cmdName[i]);

	// On retire le nom de la commande des arguments
	args.erase(args.begin());

	// 3. Exécution via la Map
	if (_cmds.find(cmdName) != _cmds.end()) {
		try {
			// Appel de la fonction membre pointée
			(this->*_cmds[cmdName])(fd, args);
		} catch (std::exception &e) {
			std::cerr << "Error exec : " << e.what() << std::endl;
		}
	} else {
		std::cerr << "Unknown command : " << cmdName << std::endl;
	}
}