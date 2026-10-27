#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Numerics.hpp"

void	Server::execPass(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, " :Not enough parameters");
		return ;
	}
	if (client->getIsRegistered() == true)
	{
		sendNumeric(client, ERR_ALREADYREGISTERED, " :Already registered");
		return ;
	}
	if (args[1] != this->_password)
	{
		sendNumeric(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		return ;
	}
	client->setHasPass(true);
}
