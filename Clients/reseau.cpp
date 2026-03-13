#include "../Client.hpp"

void Client::appendToInputBuffer(const char *data, int bytes)
{
    _inputBuffer.append(data, bytes);
}

bool Client::hasCompleteLine() const
{
    // return (_inputBuffer.find("\r\n") != std::string::npos);
    return (_inputBuffer.find("\n") != std::string::npos);
}

// Pour IRC une ligne est strictement terminee par la "\r\n"
std::string Client::extractLine()
{
    // size_t pos = _inputBuffer.find("\r\n");
    size_t pos = _inputBuffer.find("\n");

    if (pos == std::string::npos)
        return "";

    std::string line = _inputBuffer.substr(0, pos);
    _inputBuffer.erase(0, pos + 1);
    return line;
}

const std::string &Client::getInputBuffer() const
{
    return _inputBuffer;
}

const std::string &Client::getOutputBuffer() const
{
    return _outputBuffer;
}
