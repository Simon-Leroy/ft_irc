#include "Server.hpp"

Server::Server(int port) : _serverFd(-1), _port(port)
{
}

Server::~Server()
{
    if (_serverFd >= 0)
        close(_serverFd);

    for (size_t i = 1; i < _pfds.size(); i++)
        close(_pfds[i].fd);
}

void Server::initSocket()
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("socket failed");

    sockaddr_in addr = {}; // permet d'init tous les elements a 0
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");

    if (listen(_serverFd, 10) < 0)
        throw std::runtime_error("listen failed");
}

void Server::run()
{
    pollfd serverPoll;
    serverPoll.fd = _serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    _pfds.push_back(serverPoll);

    while (true)
    {
        if (poll(_pfds.data(), _pfds.size(), -1) < 0)
            throw std::runtime_error("poll failed");

        if (_pfds[0].revents & POLLIN)
            acceptNewClient();

        for (size_t i = 1; i < _pfds.size(); i++)
        {
            if (_pfds[i].revents & POLLIN)
                handlePollEvent(i);
        }
    }
}

void Server::acceptNewClient()
{
    int clientFd = accept(_serverFd, NULL, NULL);
    if (clientFd < 0)
        return;

    std::cout << "Nouveau client connecté, fd = " << clientFd << std::endl;

    pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;

    _pfds.push_back(clientPoll);
}

// void Server::handlePollEvent(size_t i)
// {
//     char buffer[1024];
//     int bytes = recv(_pfds[i].fd, buffer, sizeof(buffer) - 1, 0);

//     if (bytes <= 0)
//     {
//         std::cout << "Client déconnecté, fd = " << _pfds[i].fd << std::endl;
//         close(_pfds[i].fd);
//         _pfds.erase(_pfds.begin() + i);
//         return;
//     }

//     buffer[bytes] = '\0';
//     std::cout << "Message reçu de fd " << _pfds[i].fd << " : " << buffer << std::endl;
// }
