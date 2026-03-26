#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void Server::execQuit(Client* client, const std::vector<std::string>& args)
{
	int fd = client->getFd();
	std::string reason = "";
	if (args.size() > 1)
		reason = args[1];
	else
		reason = "Client quit";
	std::string quitMsg = ":" + client->getNickname() + "!" + client->getUsername()
						+ "@" + this->_serverName + " QUIT :" + reason + "\r\n";

	this->notifyAllChans(client, quitMsg);

	for (size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			this->removeClient(i);
			break ;
		}
	}
}
