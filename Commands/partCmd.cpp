#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void Server::execPart(Client* client, const std::vector<std::string>& args)
{
	Channel* chan = getValidChannel(client, args, 2, 1, "PART");
	if (!chan)
		return ;
	std::string reason;
	if (args.size() > 2)
		reason = args[2];
	else
		reason = "Leaving";
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + this->_serverName + " PART " + chan->getChannelName()
					+ " " + reason + "\r\n";
	// chan->broadcastMsg(NULL, msg);
	broadtoChannel(chan, NULL, msg);
	chan->kickClient(client->getFd());
	client->removeChannel(chan);
	if (chan->getNumberMembers() == 0)
		deleteChannel(chan->getChannelName());
}
