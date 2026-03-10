#include "Server_simon.hpp"

int main()
{
	Server serv;

	std::string message = "  privmsg simon test 1234";
	serv.processInput(4, message);

	return 0;
}
