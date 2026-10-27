#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"

void	Client::addChannel(Channel* chan)
{
	_joinedChannels.insert(chan);
}

void	Client::removeChannel(Channel* chan)
{
	_joinedChannels.erase(chan);
}

const std::set<Channel*>& Client::getChannels(void) const
{
	return this->_joinedChannels;
}
