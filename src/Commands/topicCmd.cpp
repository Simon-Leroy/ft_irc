#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Numerics.hpp"

void Server::execTopic(Client* client, const std::vector<std::string>& args)
{
	Channel* chan = getValidChannel(client, args, 2, 1, "TOPIC");
	if (!chan)
		return ;
	if (args.size() >= 3)
	{
		if (chan->hasMode('t') && !chan->isOperator(client->getFd()))
		{
			sendNumeric(client, ERR_CHANOPRIVSNEEDED, chan->getChannelName() + " :Operator privilege nedeed");
			return ;
		}
		std::string newTopic = args[2];
		chan->setTopic(newTopic, client->getNickname());
		std::string notif = ":" + client->getNickname() + "!" + client->getUsername()
							+ "@" + this->_serverName + " TOPIC " + chan->getChannelName()
							+ " :" + newTopic + "\r\n";
		// chan->broadcastMsg(NULL, notif);
		broadtoChannel(chan, NULL, notif);
	}
	else // READ TOPIC ONLY (pas besoin d'être opérateur)
	{
		if (chan->getTopic().empty())
		{
			sendNumeric(client, RPL_NOTOPIC, chan->getChannelName() + " :No topic is set");
			return ;
		}
		sendNumeric(client, RPL_TOPIC, chan->getChannelName() + " :" + chan->getTopic());
		std::string msg = chan->getChannelName() + " " + chan->getTopicSetter() + " "
						+ getReadableTime(chan->getTopicTime());
		sendNumeric(client, RPL_TOPICWHOTIME, msg.c_str());
	}
}
