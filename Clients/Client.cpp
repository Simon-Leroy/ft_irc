#include "../include/Client.hpp"

Client::Client(int fd) : _fd(fd), _isRegistered(false)
{
}

Client::~Client()
{
}
