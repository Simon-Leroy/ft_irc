#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

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
	std::cout << "PASS recu = [" << args[1] << "]" << std::endl;
	std::cout << "PASS attendu = [" << this->_password << "]" << std::endl;
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
						+ "127.0.0.1" + " NICK " + newNick + "\r\n";
		client->setNickname(newNick);
		notifyAllChans(client, msg);
		return ;
	}
	client->setNickname(newNick);
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
		// chan->broadcastMsg(client, msg);
		broadtoChannel(chan, client, msg);
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
		sendNumeric(client, ERR_NEEDMOREPARAMS, " :Not enough parameters");
		return ;
	}

	std::string chanName = args[1];
	if (chanName[0] != '#')
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
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
	if (chan->hasMode('i') && !chan->isClientInList(client->getNickname()))
	{
		sendNumeric(client, ERR_INVITEONLYCHAN, chanName + " :Cannot join channel (+i)");
		return ;
	}
	if (chan->hasMode('l') && chan->getNumberMembers() == chan->getLimitMembers())
	{
		sendNumeric(client, ERR_CHANNELISFULL, chanName + " :Cannot join channel (+l)");
		return ;
	}
	chan->pushClientToMembers(client);
	chan->removeFromInviteList(client->getNickname());
	client->addChannel(chan);
	chan->incrementNumberMembers();

	std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
					+ "@" + _serverName + " JOIN " + chanName + "\r\n";
	// chan->broadcastMsg(NULL, msg);
	broadtoChannel(chan, NULL, msg);
	if (!chan->getTopic().empty())
	{
		sendNumeric(client, RPL_TOPIC, chanName + " :" + chan->getTopic());
		std::string msg = chanName + " " + chan->getTopicSetter() + " "
						+ getReadableTime(chan->getTopicTime());
		sendNumeric(client, RPL_TOPICWHOTIME, msg.c_str());
	}

	//ENVOYER LA LISTE DES MEMBRES DU CHANNEL
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
	// chan->broadcastMsg(NULL, msg);
	broadtoChannel(chan, NULL, msg);
	chan->kickClient(toKick);
	toKick->removeChannel(chan);
	chan->decrementNumberMembers();
}

void Server::execPong(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
		return ;
	std::string msg = ":" + this->_serverName + " PONG " + this->_serverName + " :" + args[1] + "\r\n";
	sendResponse(client, msg);
}

void Server::execTopic(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return ;
	}

	std::string chanName = args[1];
	if (!channelExists(chanName))
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, "TOPIC :No such channel");
		return;
	}
	Channel* chan = _channels[chanName];
	if (!chan->isClientInChannel(client->getFd()))
	{
		sendNumeric(client, ERR_NOTONCHANNEL, "TOPIC :Not in channel");
		return ;
	}
	if (args.size() >= 3)
	{
		if (chan->hasMode('t') && !chan->isOperator(client->getFd()))
		{
			sendNumeric(client, ERR_CHANOPRIVSNEEDED, chanName + " :Operator privilege nedeed");
			return ;
		}
		std::string newTopic = args[2];
		chan->setTopic(newTopic, client->getNickname());
		std::string notif = ":" + client->getNickname() + "!" + client->getUsername()
							+ "@" + this->_serverName + " TOPIC " + chanName
							+ " :" + newTopic + "\r\n";
		// chan->broadcastMsg(NULL, notif);
		broadtoChannel(chan, NULL, notif);
	}
	else // READ TOPIC ONLY (pas besoin d'être opérateur)
	{
		if (chan->getTopic().empty())
		{
			sendNumeric(client, RPL_NOTOPIC, chanName + ":No topic is set");
			return ;
		}
		sendNumeric(client, RPL_TOPIC, chanName + " :" + chan->getTopic());
		std::string msg = chanName + " " + chan->getTopicSetter() + " "
						+ getReadableTime(chan->getTopicTime());
		sendNumeric(client, RPL_TOPICWHOTIME, msg.c_str());
	}
}

void Server::execInvite(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return ;
	}

	std::string chanName = args[2];
	if (!channelExists(chanName))
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, "INVITE :No such channel");
		return;
	}
	Channel* chan = _channels[chanName];
	if (!chan->isClientInChannel(client->getFd()))
	{
		sendNumeric(client, ERR_USERNOTINCHANNEL, "INVITE :Not in channel");
		return ;
	}
	if (chan->hasMode('i') && !chan->isOperator(client->getFd()))
	{
			sendNumeric(client, ERR_CHANOPRIVSNEEDED, chanName + " :Operator privilege nedeed");
			return ;
	}
	Client* target = findClientFromNick(args[1]);
	if (!target)
	{
		sendNumeric(client, ERR_NOSUCHNICK, args[1] + " :No such nick");
		return ;
	}
	if (chan->isClientInChannel(target->getFd()))
	{
		sendNumeric(client, ERR_USERONCHANNEL, target->getNickname() + " " + chanName + " :User already in channel");
		return ;
	}
	chan->addToInviteList(target->getNickname());
	std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername()
							+ "@" + this->_serverName + " INVITE " + target->getNickname()
							+ " :" + chanName + "\r\n";
	sendResponse(target, inviteMsg.c_str());
	sendNumeric(client, RPL_INVITING, target->getNickname() + " " + chanName);
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
	_commands["TOPIC"] = &Server::execTopic;
	_commands["INVITE"] = &Server::execInvite;
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
		broadStrict_toChannel(*it, client, msg, alreadySent);
	}
}
