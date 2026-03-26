#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void	Server::execKick(Client* client, const std::vector<std::string>& args)
{
	Channel* chan = getValidChannel(client, args, 3, 1, "KICK");
	if (!chan)
		return ;
	std::string targetNick = args[2];
	if (!chan->isOperator(client->getFd()))
	{
		sendNumeric(client, ERR_CHANOPRIVSNEEDED, " :Operator privilege needed");
		return ;
	}

	Client* toKick = findClientFromNick(targetNick);
	if (!toKick)
	{
		sendNumeric(client, ERR_NOSUCHNICK, " :User not found");
		return ;
	}
	if (!chan->isClientInChannel(toKick->getFd()))
	{
		sendNumeric(client, ERR_USERNOTINCHANNEL, " :User not in channel");
		return ;
	}
	std::string reason;
	if (args.size() > 3)
		reason = args[3];
	else
		reason = "Kicked by operator";
	std::string msg = ":" + client->getNickname() + " KICK " + chan->getChannelName()
					+ " " + targetNick + " :" + reason + "\r\n";
	// chan->broadcastMsg(NULL, msg);
	broadtoChannel(chan, NULL, msg);
	chan->kickClient(toKick->getFd());
	toKick->removeChannel(chan);
	// chan->decrementNumberMembers();
	if (chan->getNumberMembers() == 0)
		deleteChannel(chan->getChannelName());
}
