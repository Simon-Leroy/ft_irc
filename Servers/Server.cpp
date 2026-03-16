#include "../Server.hpp"

Server::Server(int port) : _serverFd(-1), _port(port), _password("prov"), _serverName("ft_irc.42")
{
    std::cout << "Password is '" << this->_password << "'" << std::endl;
    initCommands();
}

Server::~Server()
{
    if (_serverFd >= 0)
        close(_serverFd);

    for (size_t i = 1; i < _pfds.size(); i++)
        close(_pfds[i].fd);
}
