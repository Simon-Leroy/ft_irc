#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"


void	Server::processInput(Client* client, std::string input)
{
	if (input.empty())
	{
		std::cerr << "Missing input" << std::endl;
		return;
	}

	if(!client->getIsRegistered())
		std::cout << BLUE << "Client fd  " << client->getFd() << " -> " << input << RESET << std::endl;
	else
		std::cout << "Client fd  " << client->getFd() << " -> " << input << std::endl;

	std::string trimmed_input = ft_trim(input);
	std::vector<std::string> tokens = ft_split(trimmed_input);
	if (tokens.empty())
	{
		std::cerr << "Token vector is empty" << std::endl;
		return;
	}
	std::string cmdName = str_toupper(tokens[0]);
	if (!client->getIsRegistered()
		&& cmdName != "PASS" && cmdName != "NICK" && cmdName != "USER" && cmdName != "CAP")
	{
		sendNumeric(client, ERR_NOTREGISTERED, ":Not registered");
		return ;
	}
	if (_commands.count(cmdName))
	{
		(this->*_commands[cmdName])(client, tokens);
	}
	else
	{
		std::string msg = tokens[0] + " :Unknown command";
		sendNumeric(client, ERR_UNKNOWNCOMMAND, msg);
		return ;
	}
}

Client* Server::findClientFromNick(const std::string& nick) const
{
	std::map<int, Client*>::const_iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return it->second;
	}
	return NULL;
}

bool Server::isNickUsed(const std::string& nick) const
{
	if (nick.empty())
		return false;
	std::map<int, Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return true;
	}
	return false;
}

bool Server::channelExists(const std::string& channelName) const
{
	return (_channels.find(channelName) != _channels.end());
}

bool isValidChanName(const std::string& name) {
	if (name.empty() || name.length() > 50) // 50 est la limite standard
		return false;
	if (name[0] != '#')
		return false;
	for (size_t i = 0; i < name.length(); ++i)
	{
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\a' || name[i] == '\r' || name[i] == '\n')
			return false;
	}
	return true;
}
