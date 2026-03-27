#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

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
	_commands["PING"] = &Server::execPing;
	_commands["TOPIC"] = &Server::execTopic;
	_commands["INVITE"] = &Server::execInvite;
	_commands["PART"] = &Server::execPart;
	_commands["MODE"] = &Server::execMode;
	_commands["QUIT"] = &Server::execQuit;
}
