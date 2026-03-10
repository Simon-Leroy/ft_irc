#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>

#include <sys/socket.h>   // socket(), bind(), accept()
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // htons()
#include <unistd.h>       // close()

#include <poll.h>         // poll(), struct pollfd

#include <cstring>        // memset (optionnel)
#include <cstdlib>        // NULL

class Server
{
private:
    int _serverFd;
    int _port;
    std::vector<pollfd> _pfds;

public:
    Server(int port);
    ~Server();

    void initSocket();
    void run();
    void acceptNewClient();
    // void handlePollEvent(size_t i);
    //penser a une fonction permettant de ne pas bloquer le fd serveur/client (ca evite de bloquer le flux pcq en att de msg sur 1 fd specifique)
};

#endif