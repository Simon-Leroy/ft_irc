#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;

    int port = std::atoi(av[1]);
    Server server(port);
    server.initSocket();
    server.run();
    return 0;
}
// attributs vecteurs a modifier ?
//
// non-blocking + buffers + passage des lignes au protocole + envoi + gestion des erreurs
//
