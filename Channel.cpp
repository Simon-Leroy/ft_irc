#include "include/Channel.hpp"
#include "include/Client.hpp"

Channel::Channel(const std::string& channelName)
    : _channelName(channelName),
      _topic(""),
      _topicSetter(""),
      _topicTime(0),
      _limitMembers(0)
{
    _modes['t'] = true;
    _modes['i'] = false;
    _modes['k'] = false;
}

Channel::~Channel()
{

}

void Channel::pushClientToMembers(int fd)
{
	_members.insert(fd);
}

void Channel::kickClient(int fd)
{
    _members.erase(fd);
    _operatorsFd.erase(fd);
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
	return _members.size();
}

size_t	Channel::getLimitMembers(void) const
{
	return this->_limitMembers;
}

// void	Channel::incrementNumberMembers(void)
// {
// 	this->_numberMembers++;
// }

// void	Channel::decrementNumberMembers(void)
// {
// 	this->_numberMembers--;
// }

const std::set<int>& Channel::getMembers() const
{
	return _members;
}