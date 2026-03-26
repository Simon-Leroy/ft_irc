#include "include/Server.hpp"

// 1. Déclarer une variable globale pour l'état du serveur
bool g_running = true;

// 2. Créer le gestionnaire de signal (Handler)
void signalHandler(int signum) {
    (void)signum; // Éviter le warning unused
    std::cout << "\nArrêt propre du serveur..." << std::endl;
    g_running = false;
}

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;

    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);
    int port = std::atoi(av[1]);
    try
    {
        Server server(port);
        server.initSocket();
        while (g_running)
            server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

// attributs vecteurs a modifier ?
//
// non-blocking + buffers + passage des lignes au protocole + envoi + gestion des erreurs
//
