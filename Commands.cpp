/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamini <ahamini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 15:46:43 by ahamini           #+#    #+#             */
/*   Updated: 2026/01/06 01:52:59 by ahamini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

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
			// Extract the word between the actual position and the next space
			args.push_back(input.substr(pos, next_space - pos));
			// Put the pos at the space position
			pos = next_space;
		}
	}
	return args;
}

void Server::sendResponse(int fd, std::string response) {
	// Verify if the client still exist
	if (_clients.find(fd) == _clients.end()) {
		std::cerr << "[ERROR] sendResponse: Client " << fd << " not found." << std::endl;
		return;
	}

	if (response.empty())
		return;

	size_t total_sent = 0;
	size_t bytes_left = response.length();
	const char *ptr = response.c_str();

	while (total_sent < response.length()) {
		// MSG_NOSIGNAL : Protect from crach when a client disconnect while sending message
		ssize_t sent = send(fd, ptr + total_sent, bytes_left, MSG_NOSIGNAL);

		if (sent == -1) {
			std::cerr << "[ERROR] Failed to send data to client " << fd << std::endl;
			break;
		}

		total_sent += sent;
		bytes_left -= sent;
	}
	// Log optionnel pour debug (très utile)
	// std::cout << ">> To Client " << fd << ": " << response; 
}

void Server::sendWelcome(int fd) {
	Client *client = &_clients[fd];

	std::string nick = client->getNickname();
	std::string user = client->getUsername();
	std::string host = client->getIPAdress(); 
	// Si host est vide (cas rare), on peut mettre "localhost" par sécurité
	if (host.empty()) host = "localhost";

	// Code 001 : Welcome
	// Résultat : :localhost 001 Tonio :Welcome ... Tonio!tonio@127.0.0.1
	std::string msg = ":localhost 001 " + nick + " :Welcome to the IRC Network, " + nick + "!" + user + "@" + host + "\r\n";
	sendResponse(fd, msg);

	std::cout << "[SUCCESS] Welcome message sent to " << nick << " (" << host << ")" << std::endl;
}

Client	*Server::getClientByNickname(const std::string &nickname) {
	std::map<int, Client>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getNickname() == nickname) {
			return &(it->second);
		}
	}
	return (NULL);
}

Channel *Server::getChannel(const std::string &name) {
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end()) {
		return NULL;
	}
	return it->second;
}

void	Server::cmd_join(int fd, const std::vector<std::string> &args) {
	Client *client = &_clients[fd];
	
	if (!client->isRegistered()) {
		sendResponse(fd, ":localhost 451 :You have not registered\r\n");
		return;
	}
	if (args.empty()) {
		sendResponse(fd, ":localhost 461 :Not enough parameters\r\n");
		return;
	}

	std::vector<std::string>	channelNames;
	std::vector<std::string>	channelKeys;
	std::stringstream			ss(args[0]);
	std::string					token;

	while (std::getline(ss, token, ',')) {
		if (!token.empty() && token.size() < 200 && 
			token.find(' ') == std::string::npos &&
			token.find(',') == std::string::npos &&
			token.find('\x07') == std::string::npos)
			channelNames.push_back(token);
		else
			sendResponse(fd, "localhost 403 " + client->getNickname() + ":No such channel\r\n");
	}

	if (args.size() > 1) {
		std::stringstream	ssKey(args[1]);
		std::string			tokenKey;
		while (std::getline(ssKey, tokenKey, ','))
			channelKeys.push_back(tokenKey);
	}
	for (size_t i = 0; i < channelNames.size(); i++) {
		std::string	actualChannel = channelNames[i];
		std::string actualKey;
		if (i < channelKeys.size())
			actualKey = channelKeys[i];
		else
			actualKey = "";
		if (actualChannel[0] != '#' && actualChannel[0] != '&') {
			sendResponse(fd, "localhost 403 " + client->getNickname() + actualChannel + ":No such channel\r\n");
			continue;
		}

		Channel *channel = getChannel(actualChannel);

		if (channel && channel->isMember(client))
			continue;
		if (client->getNbChannels() > 10) {
			std::string err = ":localhost 405 " + client->getNickname() + " " + actualChannel + " :You have joined too many channels\r\n";
			sendResponse(fd, err);
			continue;
		}
		if (!channel) {
			std::cout << "DEBUG JOIN: Création de " << actualChannel << " avec Key=[" << actualKey << "]" << std::endl;
			channel = new Channel(actualChannel, "", client);
			_channels[actualChannel] = channel;
			client->addChannel(actualChannel);
		} else {
			if (!channel->getKey().empty() && channel->getKey() != actualKey) {
				sendResponse(fd, ":localhost 475 " + client->getNickname() + " " + actualChannel + " :Cannot join channel (+k)\r\n");
				continue;
			}
			channel->addClient(client);
			client->addChannel(actualChannel);
		}
		std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIPAdress() + " JOIN " + actualChannel + "\r\n";
		channel->broadcast(joinMsg, -1);

		if (!channel->getTopic().empty())
			sendResponse(fd, ":localhost 332 " + client->getNickname() + " " + actualChannel + " :" + channel->getTopic() + "\r\n");
		else
			sendResponse(fd, ":localhost 331 " + client->getNickname() + " " + actualChannel + " :No topic is set\r\n");
		std::string userList = "";
        std::vector<Client *> clientsInChan = channel->getClients(); 
        for (size_t j = 0; j < clientsInChan.size(); j++) {
            if (channel->isOperator(clientsInChan[j]))
                userList += "@";
            userList += clientsInChan[j]->getNickname();
            if (j < clientsInChan.size() - 1)
                userList += " ";
        }
        
        sendResponse(fd, ":localhost 353 " + client->getNickname() + " = " + actualChannel + " :" + userList + "\r\n");
        sendResponse(fd, ":localhost 366 " + client->getNickname() + " " + actualChannel + " :End of /NAMES list.\r\n");
	}
}

void	Server::cmd_prvmsg(int fd, const std::vector<std::string> &args) {
	Client *client = &_clients[fd];

	if (!client->isRegistered()) {
		sendResponse(fd, ":localhost 451 :You have not registered\r\n");
		return;
	}
	if (args.empty()) {
		sendResponse(fd, ":localhost 411 :No recipient given\r\n");
		return;
	}
	if (args.size() < 2) {
		sendResponse(fd, ":localhost 412 :No text to send\r\n");
		return;
	}

	std::string	clientList = args[0];
	std::string	message = args[1];
	if (message[0] == ':')
		message.erase(0, 1);
	for (size_t i = 2; i < args.size(); i++) {
		message += " " + args[i];
	}

	std::stringstream ss(clientList);
	std::string clientTarget;

	while (std::getline(ss, clientTarget, ',')) {
		if (clientTarget[0] == '#' || clientTarget[0] == '&') {
			Channel *chan = getChannel(clientTarget);

			if (!chan) {
				sendResponse(fd, ":localhost 403 " + client->getNickname() + " " + clientTarget + " :No such channel\r\n");
				continue;
			}
			if (!chan->isMember(client)) {
				sendResponse(fd, ":localhost 404 " + client->getNickname() + " " + clientTarget + " :Cannot send to channel\r\n");
				continue;
			}
			std::string fullMessage = ":" + client->getNickname() + "!" 
									+ client->getUsername() + "@" + client->getIPAdress() 
									+ " PRIVMSG " + clientTarget + " :" + message + "\r\n";
			chan->broadcast(fullMessage, fd);
		}
		else {
			Client	*recipient = getClientByNickname(clientTarget);
			if (recipient) {
				std::string fullMessage = ":" + client->getNickname() + "!" 
											+ client->getUsername() + "@" + client->getIPAdress() 
											+ " PRIVMSG " + clientTarget + " :" + message + "\r\n";
				sendResponse(recipient->getFd(), fullMessage);
			}
			else {
				std::string err = ":localhost 401 " + clientTarget + ":No such nick/channel\r\n";
				sendResponse(fd, err);
			}
		}
	}
}

void	Server::cmd_username(int fd, const std::vector<std::string> &args) {
	Client *client = &_clients[fd];
	
	if (!client->hasGivenPassword()) {
		sendResponse(fd, ":localhost 431 :No password given\r\n"); // Code un peu custom ou 431
		return;
	}
	if (client->isRegistered()) {
		sendResponse(fd, ":localhost 462 :You may not reregister\r\n");
		return;
	}
	if (args.size() < 4) {
		sendResponse(fd, ":localhost 461 USER :Not enough parameters\r\n");
		return;
	}

	std::string username = args[0];
	std::string realname = args[3];
	for (size_t i = 4; i < args.size(); i++) {
		realname += " " + args[i];
	}
	if (!realname.empty() && realname[0] == ':') {
		realname.erase(0, 1);
	}
	client->setUsername(username);
	client->setRealname(realname);
	if (!client->getNickname().empty() && !client->isRegistered()) {
		client->setIsRegistered(true);
		sendWelcome(fd);
	}
}


bool Server::isNickInUse(std::string const &nickname) {
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getNickname() == nickname)
			return true;
	}
	return false;
}

bool Server::isValidNickname(const std::string& nickname) {
	if (nickname.length() > 9) {
		return false;
	}

	std::string special = "[]\\`_^{}|-";
	for (size_t i = 0; i < nickname.length(); i++) {
		char c = nickname[i];
		if (!isalnum(c) && special.find(c) == std::string::npos) {
			return false;
		}
	}
	return true;
}

void	Server::cmd_nickname(int fd, const std::vector<std::string> &args) {
	Client *client = &_clients[fd];

	if (!client->hasGivenPassword()) {
		sendResponse(fd, ":localhost 431 :No password given\r\n"); // Code un peu custom ou 431
		return;
	}
	if (args.empty() || args[0].empty()) {
		sendResponse(fd, ":localhost 431 :No nickname given\r\n");
		return;
	}

	std::string newNickname = args[0];
	if (!isValidNickname(newNickname)) {
		std::string err = ":localhost 432 " + newNickname + " :Erroneous nickname\r\n";
		sendResponse(fd, err);
		return;
	}
	if (isNickInUse(newNickname)) {
		std::string err = ":localhost 433 * " + newNickname + " :Nickname is already in use\r\n";
		sendResponse(fd, err);
		return;
	}
	if (client->isRegistered()) {
		std::string oldNick = client->getNickname();
		client->setNickname(newNickname);
		std::string identity = oldNick + "!" + client->getUsername() + "@" + client->getIPAdress();
		std::string msg = ":" + identity + " NICK :" + newNickname + "\r\n";
		sendResponse(fd, msg);
		// TODO: Plus tard, il faudra envoyer ce message aux channels communs aussi
	} else {
		client->setNickname(newNickname);
		std::cout << "[INFO] Client " << fd << " set nickname to " << newNickname << std::endl;
	}

	if (!client->isRegistered() && !client->getUsername().empty()) {
		client->setIsRegistered(true);
		sendWelcome(fd);
	}
}

void	Server::cmd_password(int fd, const std::vector<std::string> &args) {
	Client *client = &_clients[fd];

	if (args.empty()) {
		sendResponse(fd, ":localhost 461 PASS: Not enough parameters\r\n");
		return;
	}
	if (client->isRegistered() || client->hasGivenPassword()) {
		sendResponse(fd, ":localhost 462 :You may not reregister\r\n");
		return;
	}
	std::string password_given = args[0];
	if (password_given == _password) {
		client->setHasGivenPassword(true);
		std::cout << "[SUCCESS] Client " << fd << " password accepted." << std::endl;
	} 
	else {
		client->setHasGivenPassword(false);
		sendResponse(fd, ":localhost 464 :Password incorrect\r\n");
		std::cout << "[FAIL] Client " << fd << " wrong password: " << password_given << std::endl;
	}
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