#ifndef SERVER_SIMON_HPP
#define SERVER_SIMON_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <cctype>

class Client;

class Server
{
	private:
		typedef void (Server::*cmd_ptr)(Client* client, const std::vector<std::string>& args);
		std::map<std::string, cmd_ptr>	_commands;
		std::map<int, Client*>			_clients;
		std::string						_password;

	public:
		// Constructor and destructor
		Server(std::string pass);
		~Server(void);

		// Member functions
		void	initCommands(void);
		void	processInput(Client* client, std::string input);

		// Check nickname
		bool	isNickUsed(const std::string& nick) const;

		// Command functions
		void	execNick(Client* client, const std::vector<std::string>& args);
		void	execPass(Client* client, const std::vector<std::string>& args);
		void	execUser(Client* client, const std::vector<std::string>& args);
		void	execPrivmsg(Client* client, const std::vector<std::string>& args);

		void	registerClient(Client* client);

};

std::string	str_toupper(std::string str);
std::string	ft_trim(const std::string& str);
std::vector<std::string> ft_split(const std::string& line);

#endif
