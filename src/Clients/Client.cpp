#include "../../include/Client.hpp"

Client::Client(int fd)
    : _fd(fd),
      _hasNickname(false),
      _hasPassword(false),
      _hasUser(false),
      _isRegistered(false)
{
}

Client::~Client()
{

}

/* ==== SETTERS ==== */

void Client::setNickname(const std::string& nick)
{
	this->_nickname = nick;
}

void Client::setUsername(const std::string& username)
{
	this->_username = username;
}

void Client::setRealname(const std::string& realname)
{
	this->_realname = realname;
}

void Client::setHasNickname(bool status)
{
	this->_hasNickname = status;
}

void Client::setHasPass(bool status)
{
	this->_hasPassword = status;
}

void Client::setHasUser(bool status)
{
	this->_hasUser = status;
}

void Client::setIsRegistered(bool status)
{
	this->_isRegistered = status;
}


/* ==== GETTERS ==== */

bool Client::getHasPass(void) const
{
	return this->_hasPassword;
}

bool Client::getIsRegistered(void) const
{
	return this->_isRegistered;
}

bool Client::getHasNickname(void) const
{
	return this->_hasNickname;
}

bool Client::getHasUser(void) const
{
	return this->_hasUser;
}

std::string &Client::getInputBuffer()
{
    return _inputBuffer;
}

std::string& Client::getOutputBuffer()
{
    return _outputBuffer;
}

std::string Client::getNickname(void) const
{
	return this->_nickname;
}

std::string Client::getUsername(void) const
{
	return this->_username;
}

int Client::getFd(void) const
{
	return this->_fd;
}
