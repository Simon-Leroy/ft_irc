#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& channelName): _channelName(channelName)
{
	this->_modes['t'] = true;
	this->_modes['i'] = false;
	this->_modes['k'] = false;
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

void Channel::setTopic(const std::string& newTopic, const std::string& setter)
{
	this->_topic = newTopic;
	this->_topicSetter = setter;
	this->_topicTime = std::time(0);
}

const std::string& Channel::getTopic() const
{
	return this->_topic;
}

const std::string& Channel::getTopicSetter() const
{
	return this->_topicSetter;
}

long Channel::getTopicTime() const
{
	return this->_topicTime;
}
bool Channel::hasMode(char mode)
{
	if (this->_modes.count(mode) > 0)
	{
		return _modes[mode];
	}
	return false;
}

void Channel::addToInviteList(std::string nickname)
{
	_inviteList.insert(nickname);
}

void Channel::removeFromInviteList(std::string nickname)
{
	_inviteList.erase(nickname);
}

bool Channel::isClientInList(std::string nickname)
{
	return (_inviteList.find(nickname) != _inviteList.end());
}

size_t	Channel::getNumberMembers(void) const
{
	return this->_numberMembers;
}

size_t	Channel::getLimitMembers(void) const
{
	return this->_limitMembers;
}

void	Channel::incrementNumberMembers(void)
{
	this->_numberMembers++;
}
void	Channel::decrementNumberMembers(void)
{
	this->_numberMembers--;
}
