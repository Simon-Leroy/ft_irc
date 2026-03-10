#ifndef SERVER_SIMON_HPP
#define SERVER_SIMON_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <cctype>

class Server
{
	private:
		typedef void (Server::*cmd_ptr)(int fd, const std::vector<std::string>& args);
		std::map<std::string, cmd_ptr>	_commands;

	public:
		// Constructor and destructor
		Server(void);
		~Server(void);

		// Member functions
		void	initCommands(void);
		void	processInput(int fd, std::string input);

		// Command functions
		void	execNick(int fd, const std::vector<std::string>& args);
		void	execPrivmsg(int fd, const std::vector<std::string>& args);

};

std::string str_toupper(std::string str);
std::string	ft_trim(const std::string& str);
std::vector<std::string> ft_split(const std::string& line);

#endif
