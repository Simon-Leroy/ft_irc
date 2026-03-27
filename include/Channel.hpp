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
		// General
		std::string				_channelName;
		std::set<int>			_members;
		std::set<std::string>	_inviteList;
		std::set<int>			_operatorsFd;

		// Topic
		std::string				_topic;
		std::string				_topicSetter;
		time_t					_topicTime;

		// Modes
		std::map<char, bool>	_modes;
		size_t					_limitMembers;
		std::string				_key;

	public:
		// Constructor and destructor
		Channel(const std::string& channelName);
		~Channel(void);

		// General getters
		const std::string&		getChannelName(void) const;
		size_t					getNumberMembers(void) const;
		const std::set<int>&	getMembers() const;

		// Client management
		void					pushClientToMembers(int fd);
		void					kickClient(int fd);
		bool					isClientInChannel(int fd) const;

		// Operators management
		bool					isOperator(int fd) const;
		void					addOperator(int fd);
		void					removeOperator(int fd);

		// Invite list handling
		void					addToInviteList(std::string nickname);
		void					removeFromInviteList(std::string nickname);
		bool					isClientInList(std::string nickname);

		// Topic handling
		void					setTopic(const std::string& newTopic, const std::string& setter);
		const std::string&		getTopic(void) const;
		const std::string&		getTopicSetter(void) const;
		time_t					getTopicTime(void) const;

		// Modes handling
		const std::string&		getKey(void) const;
		void					setKey(std::string key);
		void					setMode(char mode, bool state);
		bool					hasMode(char mode);
		size_t					getLimitMembers(void) const;
		void					setLimitMembers(size_t	limit);
		std::string 			activeModesToStr(void);
};

// Utils
std::string	getReadableTime(time_t rawtime);

#endif
