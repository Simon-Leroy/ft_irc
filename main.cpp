#include "include/Server.hpp"

int main(int ac, char **av)
{
    try
    {
        if (ac != 2)
            return 1;

        int port = std::atoi(av[1]);
        Server server(port);
        server.initSocket();
        server.run();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
// attributs vecteurs a modifier ?
//
// non-blocking + buffers + passage des lignes au protocole + envoi + gestion des erreurs
//
