// Rodrigo Custodio

#include <iostream>
#include <string>

#include "game/game.hpp"

int main(int argc, char *argv[])
{
	if (argc != 5) {
		std::cout << "Usage: <host> <port> "
			     "<name> <tournament_id>" << std::endl;
		return 1;
	}
	reversi::game g(argv[1], std::stoi(argv[2]));
	g.start(argv[3], std::stoi(argv[4]));
	return 0;
}
