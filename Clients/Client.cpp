#include "../include/Client.hpp"

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
