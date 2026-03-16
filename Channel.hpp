#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include <set>
#include <sys/socket.h>

class Client;

class Channel
{
	private:
		std::string				_channelName;
		std::map<int, Client*>	_members;
		std::set<int>			 _operatorsFd;

	public:
		Channel(const std::string& channelName);
		~Channel(void);

		void	pushClientToMembers(Client* client);
		void	kickClient(Client* client);
		void	broadcastMsg(Client* sender, const std::string& msg);
		void	broadcastMsgStrict(Client* sender, const std::string& msg, std::set<int>& alreadySent);
		bool	isClientInChannel(int fd) const;

		bool	isOperator(int fd) const;
		void	addOperator(int fd);
		void	removeOperator(int fd);


};


#endif
