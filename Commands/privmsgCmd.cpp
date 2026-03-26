#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void	Server::execPrivmsg(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NORECIPIENT, " :No recipient given");
		return ;
	}
	if (args.size() < 3 || args[2].empty())
	{
		sendNumeric(client, ERR_NOTEXTTOSEND, " :No text to send");
		return ;
	}

	std::string target = args[1];
	std::string text = args[2];
	std::string senderPrefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + this->_serverName;

	if (target[0] == '#')
	{
		if (!channelExists(target))
		{
			sendNumeric(client, ERR_NOSUCHCHANNEL, " :No such channel");
			return ;
		}
		Channel* chan = _channels[target];
		if (!chan->isClientInChannel(client->getFd()))
		{
			sendNumeric(client, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
			return ;
		}
		std::string msg = senderPrefix + " PRIVMSG " + target + " :" + text + "\r\n";
		// chan->broadcastMsg(client, msg);
		broadtoChannel(chan, client, msg);
	}
	else
	{
		Client* recipient = findClientFromNick(args[1]);
		if (!recipient)
		{
			sendNumeric(client, ERR_NOSUCHNICK, target + " :Recipient not found");
			return ;
		}
		std::string msg = senderPrefix + " PRIVMSG " + recipient->getNickname() + " :" + text + "\r\n";
		sendResponse(recipient, msg);
	}
}
