#include "Client.hpp"

/* ==== CONSTRUCTOR AND DESTRUCTOR ==== */

Client::Client(int fd): _fd(fd), _isRegistered(false)
{

}

Client::~Client(void)
{

}

/* ==== MEMBER FUNCTIONS ==== */

/* ==== GETTERS AND SETTERS ==== */

void Client::setNickname(const std::string& nick)
{
	this->_nickname = nick;
}

void Client::setHasNickname(bool status)
{
	this->_isRegistered = status;
}

void Client::setHasPass(bool status)
{
	this->_isRegistered = status;
}

void Client::setHasUser(bool status)
{
	this->_isRegistered = status;
}

void Client::setIsRegistered(bool status)
{
	this->_isRegistered = status;
}

std::string Client::getNickname(void) const
{
	return this->_nickname;
}

int Client::getFd(void) const
{
	return this->_fd;
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
