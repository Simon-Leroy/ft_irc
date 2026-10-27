#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Numerics.hpp"

void	Server::execCap(Client* client, const std::vector<std::string>& args)
{
	if (args.size() > 1 && args[1] == "LS")
	{
		std::string msg = ":" + this->_serverName + " CAP * LS :\r\n";
		sendResponse(client, msg);
	}
}
