// Rodrigo Custodio

#include <iostream>
#include <utility>

#include "ai.hpp"

reversi::ai::ai() = default;

void reversi::ai::set_board(int *board)
{
	this->board = board;
}

void reversi::ai::print_board()
{
	std::cout << "Board" << std::endl;
	for (unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		if (i % 8 == 0) {
			std::cout << std::endl;
		}
		std::cout << " " << board[i] << " ";
	}
	std::cout << std::endl;
}

int reversi::ai::predict_move()
{
	return 26;
}

