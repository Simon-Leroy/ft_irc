#include "Server_simon.hpp"
#include "Client.hpp"

Server::Server(std::string pass): _password(pass)
{
	initCommands();
}

Server::~Server()
{

}

bool Server::isNickUsed(const std::string& nick) const
{
	std::map<int, Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return true;
	}
	return false;
}
