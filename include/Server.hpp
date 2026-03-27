#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // USER <username> 0 * :<realname>
#include <vector>

#include <sys/socket.h>   // socket(), bind(), accept()
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // htons()
#include <unistd.h>       // close()

#include <poll.h>         // poll(), struct pollfd

#include <cstring>        // memset (optionnel)
#include <cstdlib>        // NULL
#include <map>
#include <set>
#include <cerrno>

#include <fcntl.h>
#include <csignal>

#include "Channel.hpp"
#include "Client.hpp"

class Client;
class Channel;

extern bool g_running;

class Server
{
	private:
		int _serverFd;
		int _port;
		std::vector<pollfd> _pfds; // stock tous les fd et permet a ft poll de checker

		std::map<int, Client*>			_clients;
		std::map<std::string, Channel*>	_channels;
		std::string						_password;
		std::string						_serverName;

		// Commands handling
		typedef void (Server::*cmd_ptr)(Client* client, const std::vector<std::string>& args);
		std::map<std::string, cmd_ptr>	_commands;

	public:

		Server(int port, std::string pass);
		~Server();

		// Server processes
		void		initSocket();
		void		run();
		void		initCommands(void);
		void		processInput(Client* client, std::string input);

		// Events handling
		void		handleReadEvent(size_t i);
		void		handleWriteEvent(size_t i);
		void		setPollOut(Client* client);

		// Messaging functions
		void		sendResponse(Client* client, const std::string& message);
		void		sendNumeric(Client* client, std::string code, std::string message);
		void		sendMsgToChannel(Client* sendingClient, const std::string& channelName, const std::string& msg);
		void		broadtoChannel(Channel *chan,Client* sender, const std::string& msg);
		void		broadStrict_toChannel(Channel *chan, Client* sender, const std::string& msg, std::set<int>& alreadySent);
		void		notifyAllChans(Client* client, const std::string& msg);

		// Client management
		void		acceptNewClient();
		void		registerClient(Client* client);
		void		removeClient(size_t i);
		bool		isNickUsed(const std::string& nick) const;
		Client*		findClientFromNick(const std::string& nick) const;

		// Channel management
		bool		channelExists(const std::string& channelName) const;
		void		createChannel(Client* client, const std::string& channelName);
		void		deleteChannel(std::string chanName);
		bool		canJoinChannel(Client* client, Channel* chan, const std::vector<std::string>& args);
		void		sendNamesList(Client* client, Channel* chan);

		// Command functions
		void		execJoin(Client* client, const std::vector<std::string>& args);
		void		execKick(Client* client, const std::vector<std::string>& args);
		void		execCap(Client* client, const std::vector<std::string>& args);
		void		execNick(Client* client, const std::vector<std::string>& args);
		void		execPass(Client* client, const std::vector<std::string>& args);
		void		execUser(Client* client, const std::vector<std::string>& args);
		void		execPrivmsg(Client* client, const std::vector<std::string>& args);
		void		execPing(Client* client, const std::vector<std::string>& args);
		void		execTopic(Client* client, const std::vector<std::string>& args);
		void		execInvite(Client* client, const std::vector<std::string>& args);
		void		execPart(Client* client, const std::vector<std::string>& args);
		void		execMode(Client* client, const std::vector<std::string>& args);
		void		execQuit(Client* client, const std::vector<std::string>& args);

		// Helper
		Channel*	getValidChannel(Client* client, const std::vector<std::string>& args,
									size_t min_args, size_t chanIndex, const std::string& cmd);
};

// Global utilities
std::string					str_toupper(std::string str);
std::string					ft_trim(const std::string& str);
std::vector<std::string>	ft_split(const std::string& line);

std::string					size_t_to_string(size_t n);
size_t						string_to_size_t(const std::string& str);
bool						isValidChanName(const std::string& name);

#endif
