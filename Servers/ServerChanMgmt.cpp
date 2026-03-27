#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void Server::registerClient(Client* client)
{
	client->setIsRegistered(true);
    std::cout << "------------------------------------\n" << "| Nouveau client connecté | fd = " << client->getFd() <<  " |\n------------------------------------" << std::endl;
	sendNumeric(client, RPL_WELCOME, ":Welcome to the IRC network");
}

void	Server::createChannel(Client* client, const std::string& channelName)
{
	Channel* chan = new Channel(channelName);
	chan->pushClientToMembers(client->getFd());
	chan->addOperator(client->getFd());
	client->addChannel(chan);
	_channels[channelName] = chan;
	// chan->incrementNumberMembers();
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + _serverName + " JOIN " + channelName + "\r\n";
	sendResponse(client, msg);
	sendNumeric(client, RPL_NOTOPIC, channelName + ":No topic is set");
	std::string names = "= " + channelName + " :@" + client->getNickname();
	sendNumeric(client, "353", names);
	sendNumeric(client, "366", channelName + " :End of /NAMES list");
}

Channel* Server::getValidChannel(Client* client, const std::vector<std::string>& args,
								size_t min_args, size_t chanIndex, const std::string& cmd)
{
	if (args.size() < min_args)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
		return NULL;
	}
	std::string chanName = args[chanIndex];
	if (!channelExists(chanName))
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
		return NULL;
	}
	if (!isValidChanName(chanName))
	{
		sendNumeric(client, ERR_BADCHANMASK, chanName + " :Bad Channel Mask");
		return NULL;
	}
	Channel* chan = _channels[chanName];
	if (!chan->isClientInChannel(client->getFd()))
	{
		sendNumeric(client, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
		return NULL;
	}
	return chan;
}

void Server::deleteChannel(std::string chanName)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(chanName);
	if (it != _channels.end())
	{
		delete it->second;
		_channels.erase(chanName);
	}
}
