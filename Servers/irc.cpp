#include "../Server.hpp"
#include "../Client.hpp"
#include "../Channel.hpp"
#include "../Numerics.hpp"

void	Server::sendResponse(Client *client, const std::string& message)
{
	send(client->getFd(), message.c_str(), message.length(), 0);
}

void	Server::sendNumeric(Client* client, std::string code, std::string message)
{
	std::string nick = client->getNickname();
	if (nick.empty())
		nick = "*";
	std::string finalMsg = ":" + this->_serverName + " " + code + " " + nick + " " + message + "\r\n";
	send(client->getFd(), finalMsg.c_str(), finalMsg.length(), 0);
}

void Server::registerClient(Client* client)
{
	client->setIsRegistered(true);
	std::string nick = client->getNickname();

	sendNumeric(client, RPL_WELCOME, ":Welcome to the IRC network");
/* 	std::cout << "Client " << client->getFd() << " is registered as " << client->getNickname() << std::endl; */
}


///////////////////////////////////////////////////////////////////////////////////
// COMMANDS.CPP

void	Server::execCap(Client* client, const std::vector<std::string>& args)
{
	if (args.size() > 1 && args[1] == "LS")
		sendResponse(client, ":localhost CAP * LS :\r\n");
	else if (args.size() > 1 && args[1] == "END")
		std::cout << "CAP finished" << std::endl;
}

void	Server::execPass(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return ;
	}
	if (client->getIsRegistered() == true)
	{
		sendNumeric(client, ERR_ALREADYREGISTERED, "PASS :Already registered");
		return ;
	}
	if (args[1] != this->_password)
	{
		sendNumeric(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		return ;
	}
	client->setHasPass(true);
}

void	Server::execNick(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "NICK :Not enough parameters");
		return ;
	}
	if (!client->getHasPass())
	{
		sendNumeric(client, ERR_NOTREGISTERED, "NICK :Not registered");
		return ;
	}
	std::string newNick = args[1];
	if (isNickUsed(newNick))
	{
		sendNumeric(client, ERR_NICKNAMEINUSE, newNick + " :Nickname already in use");
		return ;
	}
	if (client->getIsRegistered())
	{
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@"
						+ this->_serverName + " NICK " + newNick + "\r\n";
		client->setNickname(args[1]);
		notifyAllChans(client, msg);
		return ;
	}
	client->setNickname(args[1]);
	client->setHasNickname(true);
}

void	Server::execUser(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	if (client->getIsRegistered())
	{
		sendNumeric(client, ERR_ALREADYREGISTERED, "USER :Already registered");
		return ;
	}
	client->setUsername(args[1]);
	client->setRealname(args[4]);
	client->setHasUser(true);
	if (client->getHasPass() && client->getHasNickname() && client->getHasUser())
		registerClient(client);
}

/* ===== PRIVMSG & CHANNELMSG ===== */

void	Server::execPrivmsg(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NORECIPIENT, "PRIVMSG :No recipient given");
		return ;
	}
	if (args.size() < 3 || args[2].empty())
	{
		sendNumeric(client, ERR_NOTEXTTOSEND, "PRIVMSG :No text to send");
		return ;
	}

	std::string target = args[1];
	std::string text = args[2];
	std::string senderPrefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + this->_serverName;

	if (target[0] == '#')
	{
		if (!channelExists(target))
		{
			sendNumeric(client, ERR_NOSUCHCHANNEL, "PRIVMSG :No such channel");
			return ;
		}
		Channel* chan = _channels[target];
		if (!chan->isClientInChannel(client->getFd()))
		{
			sendNumeric(client, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
			return ;
		}
		std::string msg = senderPrefix + " PRIVMSG " + target + " :" + text + "\r\n";
		chan->broadcastMsg(client, msg);
	}
	else
	{
		Client* recipient = findClientFromNick(args[1]);
		if (!recipient)
		{
			sendNumeric(client, ERR_NOSUCHNICK, target + " :Recipient not found");
			return ;
		}
		std::string msg = senderPrefix + " PRIVMSG " + recipient->getNickname() + " :" + text + "\r\n";
		sendResponse(recipient, msg);
	}
}

/* =============================================================================== */

/* ENCORE A GERER QUAND PARAMETRE SUPPLEMENTAIRE */
void Server::execJoin(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return ;
	}

	std::string chanName = args[1];
	if (chanName[0] != '#')
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, "JOIN :No such channel");
		return ;
	}
	if (!channelExists(chanName))
	{
		createChannel(client, chanName);
		return ;
	}

	Channel* chan = _channels[chanName];
	if (chan->isClientInChannel(client->getFd()))
		return ;
	chan->pushClientToMembers(client);
	client->addChannel(chan);

	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + _serverName + " JOIN " + chanName + "\r\n";
	chan->broadcastMsg(NULL, msg);

	//AJOUTER INFOS SUR LE SERVEUR A RENVOYER A IRSSI
}

void	Server::createChannel(Client* client, const std::string& channelName)
{
	Channel* chan = new Channel(channelName);
	chan->pushClientToMembers(client);
	chan->addOperator(client->getFd());
	client->addChannel(chan);
	_channels[channelName] = chan;
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + _serverName + " JOIN " + channelName + "\r\n";
	sendResponse(client, msg);
}


void	Server::execKick(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return ;
	}
	std::string chanName = args[1];
	std::string targetNick = args[2];

	if (!channelExists(chanName))
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, "KICK :No such channel");
		return;
	}

	Channel* chan = _channels[chanName];
	if (!chan->isClientInChannel(client->getFd()))
	{
		sendNumeric(client, ERR_NOTONCHANNEL, "KICK :User not on channel");
		return ;
	}
	if (!chan->isOperator(client->getFd()))
	{
		sendNumeric(client, ERR_CHANOPRIVSNEEDED, "KICK :Operator privilege needed");
		return ;
	}

	Client* toKick = findClientFromNick(args[2]);
	if (!toKick)
	{
		sendNumeric(client, ERR_NOSUCHNICK, "KICK :User not found");
		return ;
	}
	if (!chan->isClientInChannel(toKick->getFd()))
	{
		sendNumeric(client, ERR_USERNOTINCHANNEL, "KICK :User not in channel");
		return ;
	}
	std::string reason;
	if (args.size() > 3)
		reason = args[3];
	else
		reason = "Kicked by operator";
	std::string msg = ":" + client->getNickname() + " KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";
	chan->broadcastMsg(NULL, msg);
	chan->kickClient(toKick);
}

void Server::execPong(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
		return ;
	std::string msg = ":" + this->_serverName + " PONG " + this->_serverName + " :" + args[1] + "\r\n";
	sendResponse(client, msg);
}



/*
Fonction d'initialisation de la map contenant tous les pointeurs sur fonction
des commandes qui peuvent être utilisées
INCOMPLETE POUR LE MOMENT */
void	Server::initCommands(void)
{
	_commands["JOIN"] = &Server::execJoin;
	_commands["KICK"] = &Server::execKick;
	_commands["CAP"] = &Server::execCap;
	_commands["PASS"] = &Server::execPass;
	_commands["NICK"] = &Server::execNick;
	_commands["USER"] = &Server::execUser;
	_commands["PRIVMSG"] = &Server::execPrivmsg;
	_commands["PING"] = &Server::execPong;
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
	if (!client->getIsRegistered()
		&& cmdName != "PASS" && cmdName != "NICK" && cmdName != "USER" && cmdName != "CAP")
	{
		sendNumeric(client, ERR_NOTREGISTERED, ":Not registered");
		return ;
	}
	if (_commands.count(cmdName))
		(this->*_commands[cmdName])(client, tokens);
	else
		std::cerr << "Unknown command" << std::endl;
}


/* POTENTIELLEMENT A CHANGER POUR .find() */
Client* Server::findClientFromNick(const std::string& nick) const
{
	std::map<int, Client*>::const_iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return it->second;
	}
	return NULL;
}

bool Server::isNickUsed(const std::string& nick) const
{
	if (nick.empty())
		return false;
	std::map<int, Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNickname() == nick)
			return true;
	}
	return false;
}

bool Server::channelExists(const std::string& channelName) const
{
	return (_channels.find(channelName) != _channels.end());
}

void Server::notifyAllChans(Client* client, const std::string& msg)
{
	std::set<int> alreadySent;
	const std::set<Channel*>& joinedChans = client->getChannels();

	sendResponse(client, msg);
	alreadySent.insert(client->getFd());
	for (std::set<Channel*>::const_iterator it = joinedChans.begin(); it != joinedChans.end(); ++it)
	{
		(*it)->broadcastMsgStrict(client, msg, alreadySent);
	}
}
