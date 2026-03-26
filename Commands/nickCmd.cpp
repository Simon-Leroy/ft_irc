#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void	Server::execNick(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, " :Not enough parameters");
		return ;
	}
	if (!client->getHasPass())
	{
		sendNumeric(client, ERR_NOTREGISTERED, " :Not registered");
		return ;
	}
	std::string newNick = args[1];
	if (isNickUsed(newNick))
	{
		sendNumeric(client, ERR_NICKNAMEINUSE, newNick + " :Nickname already in use");
		return ;
	}
	if (client->getIsRegistered())
	{
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@"
						+ this->_serverName + " NICK " + newNick + "\r\n";
		client->setNickname(newNick);
		notifyAllChans(client, msg);
		return ;
	}
	client->setNickname(newNick);
	client->setHasNickname(true);
}
