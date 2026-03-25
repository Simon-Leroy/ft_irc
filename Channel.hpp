#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include <set>
#include <sys/socket.h>
#include <ctime>
#include <sstream>

class Client;

class Channel
{
	private:
		std::string				_channelName;
		std::map<int, Client*>	_members;
		std::map<char, bool>	_modes;
		std::set<int>			_operatorsFd;
		std::set<std::string>	_inviteList;
		std::string				_topic;
		std::string				_topicSetter;
		time_t					_topicTime;
		size_t					_numberMembers;
		size_t					_limitMembers;

	public:
		Channel(const std::string& channelName);
		~Channel(void);

		void	pushClientToMembers(Client* client);
		void	kickClient(Client* client);
		bool	isClientInChannel(int fd) const;

		bool	isOperator(int fd) const;
		void	addOperator(int fd);
		void	removeOperator(int fd);

		void	addToInviteList(std::string nickname);
		void	removeFromInviteList(std::string nickname);
		bool	isClientInList(std::string nickname);

		size_t	getNumberMembers(void) const;
		size_t	getLimitMembers(void) const;
		void	incrementNumberMembers(void);
		void	decrementNumberMembers(void);

		void				setTopic(const std::string& newTopic, const std::string& setter);
		const std::string&	getTopic(void) const;
		const std::string&	getTopicSetter(void) const;
		time_t				getTopicTime(void) const;
		std::map<int, Client*>& getMembers();

		bool	hasMode(char mode);


};

std::string	getReadableTime(time_t rawtime);

#endif
