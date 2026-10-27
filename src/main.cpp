#include "../include/Server.hpp"

bool g_running = true;

void signalHandler(int signum)
{
    (void)signum; // Éviter le warning unused
    std::cout << "\nServer shutting down..." << std::endl;
    g_running = false;
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Port and password of server are needed" << std::endl;
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);
    int port = std::atoi(av[1]);
    try
    {
        Server server(port, av[2]);
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
