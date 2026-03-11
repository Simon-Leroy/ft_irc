#include "Server_simon.hpp"
#include "Client.hpp"

int main()
{
	Server serv("bonjour");
	Client simon(12);

	std::string message = "  PASS bonjour";
	serv.processInput(&simon, message);

	return 0;
}
