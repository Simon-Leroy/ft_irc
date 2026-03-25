#include "../include/Server.hpp"
#include "../include/Client.hpp"

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

    // boucle infini pour ajouter les nouveaux clients
    while (true)
    {
        if (poll(_pfds.data(), _pfds.size(), -1) < 0) // -1 -> poll dort tant que aucun event 
            throw std::runtime_error("poll failed");

        if (_pfds[0].revents & POLLIN) // verif qu'un flag POLLIN (demande de connexion) est bien present dans le revents de la socket serveur
            acceptNewClient();

        // gestion de l'index manuel car removeClient replace la place de lelement supprime par un autre
        for (size_t i = 1; i < _pfds.size(); )
        {
            short revents = _pfds[i].revents;
            size_t oldSize = _pfds.size();

            if (revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                removeClient(i); // gestion des erreurs comme RFC 
                continue;
            }

            if (revents & POLLIN)
            {
                handleReadEvent(i);
            }
            //car handleReadEvent peutfaire un reove clienet donc modif lelemnt a lindex
            if (_pfds.size() != oldSize)
                continue;

            if (revents & POLLOUT)
            {
                handleWriteEvent(i);
            }
            i++;    
        }
    }
}

void Server::acceptNewClient()
{
    int clientFd = accept(_serverFd, NULL, NULL);
    if (clientFd < 0)
        return; // Ajout gestion de l'erreur

    // init tous les attributs a 0 + ajouter client a la map
    std::cout << "Nouveau client connecté, fd = " << clientFd << std::endl;

    int flags = fcntl(clientFd, F_GETFL);
    if (flags == -1)
    {
        close(clientFd);
        return;
    }
    if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(clientFd);
        return;
    }

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
void Server::handleReadEvent(size_t i)
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

    // if (client->hasCompleteLine())
    while (client->hasCompleteLine())
    {
        std::string line = client->extractLine();
        std::cout << line << "\n";
        processInput(client, line);
    }
}

void Server::handleWriteEvent(size_t i)
{
    // 1. récupérer le fd et le client
    // 2. vérifier s’il y a quelque chose à envoyer
    // 3. appeler send()
    // 4. en fontion de ce que return send, supprimer POLL OUTou continuer de conserver les donnees etc

    int fd = _pfds[i].fd;
    Client* client = _clients[fd]; // Dangereux si fd n'existe pas 

    std::string& out = client->getOutputBuffer();
    if (out.empty())
    {
        _pfds[i].events &= ~POLLOUT;
        return;
    }

    ssize_t bytesSent = send(fd, out.c_str(), out.size(), 0);
    if (bytesSent > 0)
    {
        out.erase(0, bytesSent);
        if (out.empty())
            _pfds[i].events &= ~POLLOUT;
        return;
    }

    if (bytesSent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        removeClient(i);
    }
}

void Server::sendResponse(Client* client, const std::string& message)
{
	client->appendToOutputBuffer(message);
	setPollOut(client);
}

void Server::sendNumeric(Client* client, std::string code, std::string message)
{
	std::string nick = client->getNickname();
	if (nick.empty())
		nick = "*";

	std::string finalMsg = ":" + this->_serverName + " " + code + " " + nick + " " + message + "\r\n";
	sendResponse(client, finalMsg);
}

void Server::broadtoChannel(Channel *chan,Client* sender, const std::string& msg)
{
	std::map<int, Client*>& members = chan->getMembers();

	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if (sender == NULL || it->second != sender)
			sendResponse(it->second, msg);
	}
}

void Server::broadStrict_toChannel(Channel* chan, Client* sender, const std::string& msg, std::set<int>& alreadySent)
{
	std::map<int, Client*>& members = chan->getMembers();

	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		int targetFd = it->first;

		if (sender != NULL && it->second == sender)
			continue;
		if (alreadySent.find(targetFd) == alreadySent.end())
		{
			sendResponse(it->second, msg);
			alreadySent.insert(targetFd);
		}
	}
}