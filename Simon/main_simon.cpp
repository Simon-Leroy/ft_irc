#include "Server_simon.hpp"
#include "Client.hpp"

int main()
{
	Server serv("bonjour");
	Client simon(12);

	std::string password = "  PASS bonjour";
	serv.processInput(&simon, password);
	std::string nick = "  NICK nicknicknick";
	serv.processInput(&simon, nick);
	std::string user = "  USER sleroy hostname servername :Simon Leroy";
	serv.processInput(&simon, user);

	return 0;
}
