#include "../include/Server.hpp"

void Server::setPollOut(Client* client)
{
	if (!client)
		return;

	int fd = client->getFd();

	for (size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			_pfds[i].events |= POLLOUT;
			return;
		}
	}
}
