#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void	Server::execCap(Client* client, const std::vector<std::string>& args)
{
	// if (args.size() > 1 && args[1] == "LS")
	// 	sendResponse(client, ":localhost CAP * LS :\r\n");
	// else if (args.size() > 1 && args[1] == "END")
	// 	std::cout << "Client " << client->getFd() << " : CAP finished" << std::endl;
	(void)args;
	(void)client;
}
