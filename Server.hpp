#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

#include <sys/socket.h>   // socket(), bind(), accept()
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // htons()
#include <unistd.h>       // close()

#include <poll.h>         // poll(), struct pollfd

#include <cstring>        // memset (optionnel)
#include <cstdlib>        // NULL
#include <map>

class Server
{
private:
    int _serverFd;
    int _port;
    std::vector<pollfd> _pfds; // stock tous les fd et permet a ft poll de checker
    // std::map<int, Client> _clients;

    	typedef void (Server::*cmd_ptr)(Client* client, const std::vector<std::string>& args);
		std::map<std::string, cmd_ptr>	_commands;
		std::map<int, Client*>			_clients;
		std::map<std::string, Channel*>	_channels;
		std::string						_password;
		std::string						_serverName;

public:
    Server(int port);
    ~Server();

    void initSocket();
    void run();
    void acceptNewClient();
    void removeClient(size_t i);
    void handlePollEvent(size_t i);
    //penser a une fonction permettant de ne pas bloquer le fd serveur/client (ca evite de bloquer le flux pcq en att de msg sur 1 fd specifique)

        // Member functions
		void	initCommands(void);
		void	processInput(Client* client, std::string input);
		void	sendResponse(Client* client, const std::string& message);
		void	sendNumeric(Client* client, std::string code, std::string message);

		// Check
		bool	isNickUsed(const std::string& nick) const;

		// Channel
		bool	channelExists(const std::string& channelName) const;
		void	createChannel(Client* client, const std::string& channelName);
		void	sendMsgToChannel(Client* sendingClient, const std::string& channelName, const std::string& msg);
		void	notifyAllChans(Client* client, const std::string& msg);

		Client* findClientFromNick(const std::string& nick) const;

		// Command functions
		void	execJoin(Client* client, const std::vector<std::string>& args);
		void	execKick(Client* client, const std::vector<std::string>& args);
		void	execCap(Client* client, const std::vector<std::string>& args);
		void	execNick(Client* client, const std::vector<std::string>& args);
		void	execPass(Client* client, const std::vector<std::string>& args);
		void	execUser(Client* client, const std::vector<std::string>& args);
		void	execPrivmsg(Client* client, const std::vector<std::string>& args);
		void	execPong(Client* client, const std::vector<std::string>& args);

		void	registerClient(Client* client);
};

std::string	str_toupper(std::string str);
std::string	ft_trim(const std::string& str);
std::vector<std::string> ft_split(const std::string& line);

#endif

