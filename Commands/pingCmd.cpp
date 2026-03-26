#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void Server::execPong(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
		return ;
	std::string msg = ":" + this->_serverName + " PONG " + this->_serverName + " :" + args[1] + "\r\n";
	sendResponse(client, msg);
}
