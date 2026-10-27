#include "../../include/Server.hpp"

Server::Server(int port, std::string pass) : _serverFd(-1), _port(port), _password(pass), _serverName("localhost")
{
    // std::cout << "Password is '" << this->_password << "'" << std::endl;
    initCommands();
}

Server::~Server()
{
    if (_serverFd >= 0)
    {
        close(_serverFd);
        _serverFd = -1;
    }

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->first >= 0)
            close(it->first);
        delete it->second;
    }
    _clients.clear();

    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();

    _pfds.clear();
    _commands.clear();
}

// removeClient() ne delete pas deux fois un Client*
// un Channel* n’est stocké qu’une seule fois dans _channels

// Donc il faut que tout ton code respecte bien cette propriété.
