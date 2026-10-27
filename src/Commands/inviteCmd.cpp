#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Numerics.hpp"

void Server::execInvite(Client* client, const std::vector<std::string>& args)
{
	Channel* chan = getValidChannel(client, args, 3, 2, "INVITE");
	if (!chan)
		return ;
	if (chan->hasMode('i') && !chan->isOperator(client->getFd()))
	{
			sendNumeric(client, ERR_CHANOPRIVSNEEDED, chan->getChannelName() + " :Operator privilege nedeed");
			return ;
	}
	Client* target = findClientFromNick(args[1]);
	if (!target)
	{
		sendNumeric(client, ERR_NOSUCHNICK, args[1] + " :No such nick");
		return ;
	}
	if (chan->isClientInChannel(target->getFd()))
	{
		sendNumeric(client, ERR_USERONCHANNEL, target->getNickname() + " " + chan->getChannelName() + " :User already in channel");
		return ;
	}
	chan->addToInviteList(target->getNickname());
	std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername()
							+ "@" + this->_serverName + " INVITE " + target->getNickname()
							+ " :" + chan->getChannelName() + "\r\n";
	sendResponse(target, inviteMsg.c_str());
	sendNumeric(client, RPL_INVITING, target->getNickname() + " " + chan->getChannelName());
}
