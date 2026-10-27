#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Numerics.hpp"

void Server::sendNamesList(Client* client, Channel* chan)
{
	std::string nickList;
	const std::set<int>& members = chan->getMembers();

	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		int memberFd = *it;
		std::map<int, Client*>::iterator itClient = _clients.find(memberFd);

		if (itClient == _clients.end())
			continue;

		Client* currentMem = itClient->second;

		if (!nickList.empty())
			nickList += " ";

		if (chan->isOperator(memberFd))
			nickList += "@";

		nickList += currentMem->getNickname();
	}

	sendNumeric(client, "353", "= " + chan->getChannelName() + " :" + nickList);
	sendNumeric(client, "366", chan->getChannelName() + " :End of /NAMES list");
}

bool Server::canJoinChannel(Client* client, Channel* chan, const std::vector<std::string>& args)
{
	const std::string& name = chan->getChannelName();
	if (chan->hasMode('k') && (args.size() < 3 || chan->getKey() != args[2]))
	{
		sendNumeric(client, ERR_BADCHANNELKEY, name + " :Cannot join channel (+k)");
		return false;
	}
	if (chan->hasMode('i') && !chan->isClientInList(client->getNickname()))
	{
		sendNumeric(client, ERR_INVITEONLYCHAN, name + " :Cannot join channel (+i)");
		return false;
	}

	if (chan->hasMode('l') && chan->getNumberMembers() >= chan->getLimitMembers())
	{
		sendNumeric(client, ERR_CHANNELISFULL, name + " :Cannot join channel (+l)");
		return false;
	}
	return true;
}

void Server::execJoin(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, " :Not enough parameters");
		return ;
	}

	std::string chanName = args[1];
	if (!isValidChanName(chanName))
	{
		sendNumeric(client, ERR_BADCHANMASK, chanName + " :Bad Channel Mask");
		return ;
	}
	if (!channelExists(chanName))
	{
		createChannel(client, chanName);
		return ;
	}
	Channel* chan = _channels[chanName];
	if (chan->isClientInChannel(client->getFd()))
		return ;
	if (!canJoinChannel(client, chan, args))
		return ;
	chan->pushClientToMembers(client->getFd());
	chan->removeFromInviteList(client->getNickname());
	client->addChannel(chan);
	// chan->incrementNumberMembers();

	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + _serverName + " JOIN " + chanName + "\r\n";
	// chan->broadcastMsg(NULL, msg);
	broadtoChannel(chan, NULL, msg);
	if (!chan->getTopic().empty())
	{
		sendNumeric(client, RPL_TOPIC, chanName + " :" + chan->getTopic());
		std::string msg = chanName + " " + chan->getTopicSetter() + " "
						+ getReadableTime(chan->getTopicTime());
		sendNumeric(client, RPL_TOPICWHOTIME, msg.c_str());
	}
	sendNamesList(client, chan);
}
