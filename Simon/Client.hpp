#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client
{
	private:
		int			_fd;
		std::string	_nickname;
		std::string _username;
		bool		_hasNickname;
		bool		_hasPassword;
		bool		_hasUser;
		bool		_isRegistered;

	public:
		// Constructor and destructor
		Client(int fd);
		~Client(void);

		// Getters and setters
		std::string	getNickname(void) const;
		int			getFd(void) const;
		bool		getHasPass(void) const;
		bool		getHasNickname(void) const;
		bool		getHasUser(void) const;
		bool		getIsRegistered(void) const;

		void		setNickname(const std::string& nick);

		// Registration
		void		setHasPass(bool newStatus);
		void		setHasNickname(bool newStatus);
		void		setHasUser(bool newStatus);
		void		setIsRegistered(bool newStatus);


};

#endif
