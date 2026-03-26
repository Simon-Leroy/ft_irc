#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Numerics.hpp"

std::string Channel::activeModesToStr(void)
{
	std::string flags = "+";
	std::string args = "";

	if (hasMode('i'))
		flags += 'i';
	if (hasMode('t'))
		flags += 't';
	if (hasMode('k'))
	{
		flags += 'k';
		args += " " + this->_key;
	}
	if (hasMode('l'))
	{
		flags += 'l';
		args += " " + size_t_to_string(_limitMembers);
	}
	return flags + args;
}

void	Server::execMode(Client* client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		sendNumeric(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return ;
	}
	// CAS USER MODE
	std::string target = args[1];
	if (target[0] != '#')
	{
		if (target == client->getNickname())
		{
			if (args.size() == 2)
				sendNumeric(client, RPL_UMODEIS, "+i");
			else
			{
				std::string msg = ":" + client->getNickname() + " MODE " + target + " " + args[2] + "\r\n";
				sendResponse(client, msg);
			}
		}
		else
			sendNumeric(client, ERR_USERSDONTMATCH, " :Cannot change mode for other users");
		return;
	}


	// CAS CHANNEL MODE
	std::string chanName = args[1];
	if (!isValidChanName(chanName))
	{
		sendNumeric(client, ERR_BADCHANMASK, chanName + " :Bad Channel Mask");
		return ;
	}
	if (!channelExists(chanName))
	{
		sendNumeric(client, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
		return ;
	}
	Channel* chan = _channels[chanName];
	if (args.size() == 2)
	{
		std::string response = chanName + " " + chan->activeModesToStr();
		sendNumeric(client, RPL_CHANNELMODEIS, response);
		return ;
	}
	if (!chan->isOperator(client->getFd()))
	{
		sendNumeric(client, ERR_CHANOPRIVSNEEDED, chanName + " :You're not channel operator");
		return ;
	}

	std::string flags = args[2];
	std::string validModes = "itkol";
	std::string appliedModes = "";
	std::string appliedArgs = "";
	size_t argPos = 3;
	bool adding = true;

	for (size_t i = 0; i < flags.length(); i++)
	{
		char c = flags[i];
		if (c == '+')
		{
			adding = true;
			appliedModes += "+";
			continue ;
		}
		if (c == '-')
		{
			adding = false;
			appliedModes += "-";
			continue ;
		}
		if (validModes.find(c) == std::string::npos)
		{
			sendNumeric(client, ERR_UNKNOWNMODE, std::string(1, c) + " :is unknown mode char");
			continue ;
		}

		// MODES
		if (c == 'i' || c == 't')
		{
			chan->setMode(c, adding);
			appliedModes += c;
		}
		else if (c == 'k')
		{
			if (adding && argPos < args.size())
			{
				std::string key = args[argPos++];
				chan->setKey(key);
				chan->setMode('k', true);
				appliedModes += 'k';
				appliedArgs += " " + key;
			}
			else if (!adding)
			{
				chan->setMode('k', false);
				appliedModes += 'k';
			}
		}
		else if (c == 'l')
		{
			if (adding && argPos < args.size())
			{
				int limit = std::atoi(args[argPos++].c_str());
				if (limit > 0)
				{
					chan->setLimitMembers(limit);
					chan->setMode('l', true);
					appliedModes += 'l';
					appliedArgs += " " + size_t_to_string(limit);
				}
			}
			else if (!adding)
			{
				chan->setMode('l', false);
				appliedModes += 'l';
			}
		}
		else if (c == 'o')
		{
			if (argPos < args.size())
			{
				std::string targetNick = args[argPos++];
				Client* target = findClientFromNick(targetNick);
				if (target && chan->isClientInChannel(target->getFd()))
				{
					if (adding)
						chan->addOperator(target->getFd());
					else
						chan->removeOperator(target->getFd());
					appliedModes += 'o';
					appliedArgs += " " + targetNick;
				}
				else
					sendNumeric(client, ERR_NOSUCHNICK, targetNick + " :No such nick");
			}
		}
	}
	if (appliedModes != "+" && appliedModes != "-")
	{
		std::string notify = ":" + client->getNickname() + "!" + client->getUsername()
							+ "@" + this->_serverName + " MODE " + chanName + " "
							+ appliedModes + appliedArgs + "\r\n";
		// chan->broadcastMsg(NULL, notify);
		broadtoChannel(chan, NULL, notify);
	}
}
