#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& channelName): _channelName(channelName)
{

}

Channel::~Channel()
{

}

void Channel::pushClientToMembers(Client* client)
{
	_members[client->getFd()] = client;
}

void Channel::kickClient(Client *client)
{
	_members.erase(client->getFd());
}

/*
A MODIFIER POUR AJOUTER LE MESSAGE DANS L'OUTPUT BUFFER */
void	Channel::broadcastMsg(Client* sender, const std::string& msg)
{
	std::map<int, Client*>::iterator it;
	for(it = _members.begin(); it != _members.end(); it++)
	{
		if (sender == NULL || it->second != sender)
			send(it->first, msg.c_str(), msg.length(), 0);
	}
}

void Channel::broadcastMsgStrict(Client* sender, const std::string& msg, std::set<int>& alreadySent)
{
	std::map<int, Client*>::iterator it;
	for (it = _members.begin(); it != _members.end(); it++)
	{
		int targetFd = it->first;
		if (sender != NULL && it->second == sender)
			continue;
		if (alreadySent.find(targetFd) == alreadySent.end())
		{
			send(targetFd, msg.c_str(), msg.length(), 0);
			alreadySent.insert(targetFd);
		}
	}
}

bool Channel::isClientInChannel(int fd) const
{
	return (_members.find(fd) != _members.end());
}

bool Channel::isOperator(int fd) const
{
	return (_operatorsFd.find(fd) != _operatorsFd.end());
}

void Channel::addOperator(int fd)
{
	_operatorsFd.insert(fd);
}

void Channel::removeOperator(int fd)
{
	_operatorsFd.erase(fd);
}
