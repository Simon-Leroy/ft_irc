#include "Server_simon.hpp"

/* COMMANDES POUR TESTER

Commandes qui vont être modifiées, print juste un message pour le moment */

void	Server::execNick(int fd, const std::vector<std::string>& args)
{
	std::cout << "FD: " << fd << " | Arguments reçus pour commande Nick : " << args[1] << std::endl;
}

void	Server::execPrivmsg(int fd, const std::vector<std::string>& args)
{
	std::cout << "FD: " << fd << " | Arguments reçus pour commande Privmsg : " << args[1] << std::endl;
}

/*
Fonction d'initialisation de la map contenant tous les pointeurs sur fonction
des commandes qui peuvent être utilisées
INCOMPLETE POUR LE MOMENT */
void	Server::initCommands(void)
{
	_commands["NICK"] = &Server::execNick;
	_commands["PRIVMSG"] = &Server::execPrivmsg;
}

void	Server::processInput(int fd, std::string input)
{
	if (input.empty())
	{
		std::cerr << "Missing input" << std::endl;
		return;
	}
	std::string trimmed_input = ft_trim(input);
	std::vector<std::string> tokens = ft_split(trimmed_input);
	if (tokens.empty())
	{
		std::cerr << "Token vector is empty" << std::endl;
		return;
	}
	std::string cmdName = str_toupper(tokens[0]);
	if (_commands.count(cmdName))
		(this->*_commands[cmdName])(fd, tokens);
	else
		std::cerr << "Unknown command" << std::endl;
}
