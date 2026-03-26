#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

void	Server::execUser(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, " :Not enough parameters");
		return ;
	}
	if (client->getIsRegistered())
	{
		sendNumeric(client, ERR_ALREADYREGISTERED, " :Already registered");
		return ;
	}
	client->setUsername(args[1]);
	client->setRealname(args[4]);
	client->setHasUser(true);
	if (client->getHasPass() && client->getHasNickname() && client->getHasUser())
		registerClient(client);
}
