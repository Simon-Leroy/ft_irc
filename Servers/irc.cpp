#include "../Server.hpp"
#include "../Client.hpp"

void	Server::sendResponse(Client *client, const std::string& message)
{
	send(client->getFd(), message.c_str(), message.length(), 0);
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
	std::string nick = client->getNickname();

	std::string msg = ":localhost 001 " + nick + " :Welcome to the IRC Network, " + nick + "\r\n";
	sendResponse(client, msg);
	std::cout << "Client " << client->getFd() << " is registered as " << client->getNickname() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////
// COMMANDS.CPP

void	Server::execCap(Client* client, const std::vector<std::string>& args)
{
	if (args.size() > 1 && args[1] == "LS")
	{
		std::cout << "message envoyé" << std::endl;
		sendResponse(client, ":localhost CAP * LS :\r\n");
	}
	else if (args.size() > 1 && args[1] == "END")
	{
		std::cout << "CAP finished" << std::endl;
	}
}

void	Server::execJoin(Client* client, const std::vector<std::string>& args)
{
	(void)client;
	(void)args;
	std::cout << "JOIN command received and ignored" << std::endl;
}

void	Server::execPass(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendResponse(client, "461 ERR_NEEDMOREPARAMS\r\n");
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (client->getIsRegistered() == true)
	{
		sendResponse(client, "You are already registered\n");
//		std::cerr << "462 ERR_ALREADYREGISTRED --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (args[1] != this->_password)
	{
		sendResponse(client, "Wrong password\n");
		std::cout << "464 ERR_PASSWDMISMATCH --> erreur à renvoyer au serveur" << std::endl;
		// ACTION A IMPLEMENTER? DECONNECTER LE CLIENT?
		return ;
	}
	client->setHasPass(true);
	sendResponse(client, "Password correct\r\n");
	std::cout << "Client " << client->getFd() << " | Password correct" << std::endl;
}

void	Server::execNick(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendResponse(client, "461 ERR_NEEDMOREPARAMS\n");
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (!client->getHasPass())
	{
		sendResponse(client, "451 ERR_NOTREGISTERED\n");
		std::cerr << "Client " << client->getFd() << " | 451 ERR_NOTREGISTERED" << std::endl;
		return ;
	}
	if (isNickUsed(args[1]))
	{
		sendResponse(client, "433 ERR_NICKNAMEINUSE\n");
		std::cerr << "Client " << client->getFd() << " | 433 ERR_NICKNAMEINUSE --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	if (client->getHasNickname())
	{
		client->setNickname(args[1]);
		sendResponse(client, "nickname successfully changed\n");
		std::cout << "Client " << client->getFd() << " changed his nickname to " << args[1] << std::endl;
		return ;
	}
	client->setNickname(args[1]);
	client->setHasNickname(true);
	sendResponse(client, "nickname set\n");
	std::cout << "Client " << client->getFd() << " set his nickname to " << args[1] << std::endl;
}

void	Server::execUser(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		sendResponse(client, "461 ERR_NEEDMOREPARAMS\n");
		std::cerr << "461 ERR_NEEDMOREPARAMS --> erreur à renvoyer au client" << std::endl;
		return ;
	}
	if (client->getIsRegistered())
	{
		sendResponse(client, "462 ERR_ALREADYREGISTRED\n");
		std::cerr << "462 ERR_ALREADYREGISTRED --> erreur à renvoyer au serveur" << std::endl;
		return ;
	}
	client->setUsername(args[1]);
	client->setRealname(args[4]);
	client->setHasUser(true);
	if (client->getHasPass() && client->getHasNickname() && client->getHasUser())
		registerClient(client);
}


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
	_commands["JOIN"] = &Server::execJoin;
	_commands["CAP"] = &Server::execCap;
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
