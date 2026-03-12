#include "../Server.hpp"

Server::Server(int port) : _serverFd(-1), _port(port)
{
	this->_password = "provisoire";
    initCommands();
}

Server::~Server()
{
    if (_serverFd >= 0)
        close(_serverFd);

    for (size_t i = 1; i < _pfds.size(); i++)
        close(_pfds[i].fd);
}
