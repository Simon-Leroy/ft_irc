#include "Server_simon.hpp"
#include "Client.hpp"

Server::Server(std::string pass): _password(pass)
{
	initCommands();
}

Server::~Server()
{

}

bool Server::isNickUsed(const std::string& nick) const
{
	std::map<int, Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return true;
	}
	return false;
}

void Server::registerClient(Client* client)
{
	client->setIsRegistered(true);
	std::cout << "Welcome to server blabla -> message à envoyer avec send()" << std::endl;
	std::cout << "Client " << client->getFd() << " is registered as " << client->getNickname() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////
// COMMANDS.CPP

#include "Server_simon.hpp"
#include "Client.hpp"

void	Server::execPass(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (client->getIsRegistered() == true)
	{
		std::cerr << "462 ERR_ALREADYREGISTRED --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (args[1] != this->_password)
	{
		std::cout << "464 ERR_PASSWDMISMATCH --> erreur à renvoyer au serveur" << std::endl;
		// ACTION A IMPLEMENTER? DECONNECTER LE CLIENT?
		return ;
	}
	client->setHasPass(true);
	std::cout << "Client " << client->getFd() << " | Password correct" << std::endl;
}

void	Server::execNick(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (!client->getHasPass())
	{
		std::cerr << "Client " << client->getFd() << " | 451 ERR_NOTREGISTERED" << std::endl;
		return ;
	}
	if (isNickUsed(args[1]))
	{
		std::cerr << "Client " << client->getFd() << " | 433 ERR_NICKNAMEINUSE --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (client->getHasNickname())
	{
		client->setNickname(args[1]);
		std::cout << "Client " << client->getFd() << " changed his nickname to " << args[1] << std::endl;
		return ;
	}
	client->setNickname(args[1]);
	client->setHasNickname(true);
	std::cout << "Client " << client->getFd() << " set his nickname to " << args[1] << std::endl;
}

void	Server::execUser(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au client" << std::endl;
		return ;
	}
	if (client->getIsRegistered())
	{
		std::cerr << "462 ERR_ALREADYREGISTRED --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	client->setUsername(args[1]);
	client->setRealname(args[4]);
	client->setHasUser(true);
	if (client->getHasPass() && client->getHasNickname() && client->getHasUser())
		registerClient(client);
	std::cout << "fin execUser" << std::endl;
}

/* COMMANDES POUR TESTER

Commandes qui vont être modifiées, print juste un message pour le moment */



void	Server::execPrivmsg(Client* client, const std::vector<std::string>& args)
{
	std::cout << "FD: " << client->getFd() << " | Arguments reçus pour commande Privmsg : " << args[1] << std::endl;
}

/*
Fonction d'initialisation de la map contenant tous les pointeurs sur fonction
des commandes qui peuvent être utilisées
INCOMPLETE POUR LE MOMENT */
void	Server::initCommands(void)
{
	_commands["PASS"] = &Server::execPass;
	_commands["NICK"] = &Server::execNick;
	_commands["USER"] = &Server::execUser;
	_commands["PRIVMSG"] = &Server::execPrivmsg;
}

/*
Fonction de process de l'input

1)trim le début de la string pour enlever les espaces vides
(trim aussi la fin de \\r et \\n ?)
2)tokenize la string en vecteur d'arguments (contenant notamment
la commande utilisée)
3)on passe la commande en majuscules
4)on cherche la commande dans la map et on l'exécute si elle existe,
sinon on renvoie un message d'erreur */
void	Server::processInput(Client* client, std::string input)
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
		(this->*_commands[cmdName])(client, tokens);
	else
		std::cerr << "Unknown command" << std::endl;
}
