#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // USER <username> 0 * :<realname>
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
#include <cerrno>

#include <fcntl.h>

class Client;
class Channel;

class Server
{
private:
    int _serverFd;
    int _port;
    std::vector<pollfd> _pfds; // stock tous les fd et permet a ft poll de checker

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
    void handleReadEvent(size_t i);
    void handleWriteEvent(size_t i);
	void sendResponse(Client* client, const std::string& message);
	void sendNumeric(Client* client, std::string code, std::string message);
	void broadtoChannel(Channel *chan,Client* sender, const std::string& msg);
	void broadStrict_toChannel(Channel *chan, Client* sender, const std::string& msg, std::set<int>& alreadySent);

	void setPollOut(Client* client);

    // Member functions
	void	initCommands(void);
	void	processInput(Client* client, std::string input);

		// Check
	bool	isNickUsed(const std::string& nick) const;

		// Channel
	bool	channelExists(const std::string& channelName) const;
	void	createChannel(Client* client, const std::string& channelName);
	void	sendMsgToChannel(Client* sendingClient, const std::string& channelName, const std::string& msg);
	void	notifyAllChans(Client* client, const std::string& msg);
		void	deleteChannel(std::string chanName); // NEW
		bool	canJoinChannel(Client* client, Channel* chan, const std::vector<std::string>& args); // NEW


	Client* findClientFromNick(const std::string& nick) const;

		Channel* getValidChannel(Client* client, const std::vector<std::string>& args,
								size_t min_args, size_t chanIndex, const std::string& cmd); // NEW

		// Command functions
	void	execJoin(Client* client, const std::vector<std::string>& args);
	void	execKick(Client* client, const std::vector<std::string>& args);
	void	execCap(Client* client, const std::vector<std::string>& args);
	void	execNick(Client* client, const std::vector<std::string>& args);
	void	execPass(Client* client, const std::vector<std::string>& args);
	void	execUser(Client* client, const std::vector<std::string>& args);
	void	execPrivmsg(Client* client, const std::vector<std::string>& args);
	void	execPong(Client* client, const std::vector<std::string>& args);
	void	execTopic(Client* client, const std::vector<std::string>& args);
	void	execInvite(Client* client, const std::vector<std::string>& args);
		void	execPart(Client* client, const std::vector<std::string>& args); // NEW
		void	execMode(Client* client, const std::vector<std::string>& args); // NEW

	void	registerClient(Client* client);
		void	sendNamesList(Client* client, Channel* chan); // NEW
};

std::string	str_toupper(std::string str);
std::string	ft_trim(const std::string& str);
std::vector<std::string> ft_split(const std::string& line);

	std::string size_t_to_string(size_t n); // NEW
	size_t string_to_size_t(const std::string& str); // NEW
	bool isValidChanName(const std::string& name); // NEW

#endif