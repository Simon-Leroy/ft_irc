#include "../Server.hpp"
#include "../Client.hpp"

//initier une socket (canal bi-lateral) et mettre sur ecoute sur le bon port
void Server::initSocket()
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("socket failed");

    sockaddr_in addr = {}; // permet d'init tous les elements a 0
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port); // convertir le port en bytes ordrer dureseau
    addr.sin_addr.s_addr = INADDR_ANY;

    // lier la socket a l'adresse
    if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");

    if (listen(_serverFd, 10) < 0)
        throw std::runtime_error("listen failed");
}

// ajouter notre socket serveur a struct pollfd + ecouter toutes requetes fait sur ce dernier
void Server::run()
{
    pollfd serverPoll;
    serverPoll.fd = _serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    _pfds.push_back(serverPoll);
    std::map<int, Client*>::iterator it;

    // boucle infini pour aouter les nouveaux clients
    while (true)
    {
        if (poll(_pfds.data(), _pfds.size(), -1) < 0) // ATTENTIOn, il faudrq que je passe a 0 pkutt qe -1 pour du non bloquant
            throw std::runtime_error("poll failed");

        if (_pfds[0].revents & POLLIN) // verif qu'un fag POLLIN (demande de connexion) est bien present dans le revents
            acceptNewClient();

        // si une socket contient un revent avec le flag POLLIN, on va retirer les donnees
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
// init tous les attributs a 0 + ajouter client a la map
    std::cout << "Nouveau client connecté, fd = " << clientFd << std::endl;

    pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;

    _pfds.push_back(clientPoll);
    _clients[clientFd] = new Client(clientFd);
}

void Server::removeClient(size_t i)
{
    int fd = _pfds[i].fd;

    std::cout << "Client déconnecté, fd = " << fd << std::endl;

    close(fd);                 // ferme la socket
    delete _clients[fd];
    _clients.erase(fd);        // supprime de la map
    _pfds.erase(_pfds.begin() + i); // retire de poll
}
// stocker dans BUffer les data recu du client grace a recv puis
// appeler methode de la class Client pour save toutes les donnees (le reste de limplementation se fera dans Client)
void Server::handlePollEvent(size_t i)
{
    int fd = _pfds[i].fd;
    Client* client = _clients[fd];

    char buffer[1024];
    int bytes = recv(fd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        removeClient(i);
        return;
    }

    client->appendToInputBuffer(buffer, bytes);

    while (client->hasCompleteLine())
    {
        std::string line = client->extractLine();
        std::cout << line << "\n";
        processInput(client, line);
        // handleLine(fd, line);
    }
}

