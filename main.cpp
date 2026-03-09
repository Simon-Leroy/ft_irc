#include <iostream>
#include <sys/socket.h> // Pour socket, bind, listen
#include <netinet/in.h> // Pour sockaddr_in
#include <unistd.h>     // Pour close
#include <cstring>      // Pour memset

int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "Usage: ./ircserv <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(av[1]);

    // 1. Création de la socket (IPv4, TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Configuration de l'adresse du serveur
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces
    address.sin_port = htons(port);       // Host TO Network Short (Endianness)

    // 3. Liaison de la socket au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }

    // 4. Mise en écoute
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Serveur en attente sur le port " << port << "..." << std::endl;

    // 5. Acceptation d'une connexion (bloquant pour ce proto)
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }

    std::cout << "Client connecté !" << std::endl;

    // 6. Boucle de lecture simple
    char buffer[1024];
    while (true) {
        memset(buffer, 0, 1024);
        int bytes_read = recv(client_fd, buffer, 1024, 0);

        if (bytes_read <= 0) {
            std::cout << "Client déconnecté." << std::endl;
            break;
        }

        std::cout << "Message reçu : [" << buffer << "]" << std::endl;

        // Petit clin d'oeil au partenaire B : un parsing ultra-basique
        if (std::string(buffer).find("CAP LS") != std::string::npos) {
            std::string welcome = "NOTICE :Bienvenue sur ft_irc !\r\n";
            send(client_fd, welcome.c_str(), welcome.length(), 0);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
