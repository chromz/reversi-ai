// Rodrigo Custodio

#include <iostream>
#include <utility>

#include "ai.hpp"

reversi::ai::ai() = default;

void reversi::ai::set_board(std::array<int, REVERSI_BOARD_SIZE> board)
{
	this->board = std::move(board);
}

void reversi::ai::set_tile(int tile)
{
	this->tile = tile;
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

inline int reversi::ai::pos(const std::array<int, REVERSI_BOARD_SIZE> &board,
			    int x, int y)
{
	return board[y * REVERSI_ROW_LEN + x];
}

inline void reversi::ai::set(std::array<int, REVERSI_BOARD_SIZE> &board,
	       int x, int y, int val)
{
	board[y * REVERSI_ROW_LEN + x] = val;
}

inline bool reversi::ai::is_inside(int x, int y)
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool reversi::ai::check_board(reversi::state &test,
			      int x, int y)
{
	bool is_valid = false;
	std::array<int, REVERSI_BOARD_SIZE> &test_board = test.board;
	if (pos(test_board, x, y) != 0 || !is_inside(x, y)) {
		return false;
	}
	set(test_board, x, y, tile);
	int enemy = (tile == 1) ? 2 : 1;

	const int move_size = 16;
	int moves[move_size] = {
		0, 1,
		1, 1,
		1, 0,
		1, -1,
		0, -1,
		-1, -1,
		-1, 0,
		-1, 1,
	};
	for (int i = 0; i < move_size; i += 2) {
		int xdir = moves[i];
		int ydir = moves[i + 1];
		int posx = x + xdir;
		int posy = y + ydir;
		if (is_inside(posx, posy) &&
		    pos(test_board, posx, posy) == enemy) {
			posx += xdir;
			posy += ydir;
			if (!is_inside(posx, posy)) {
				continue;
			}

			while (pos(test_board, posx, posy) == enemy) {
				posx += xdir;
				posy += ydir;
				if (!is_inside(posx, posy)) {
					break;
				}
			}

			if (!is_inside(posx, posy)) {
				continue;
			}

			if (pos(test_board, posx, posy) == tile) {
				while (true) {
					posx -= xdir;
					posy -= ydir;
					if (posx == x && posy == y) {
						break;
					}
					set(test_board, posx, posy, tile);
					is_valid = true;
				}
			}
		}
	}
	return is_valid;
}

std::vector<reversi::state> reversi::ai::get_next_states()
{
	std::vector<reversi::state> states;
	reversi::state test;
	for(int x = 0; x < REVERSI_ROW_LEN; x++) {
		for (int y = 0; y < REVERSI_COL_LEN; y++) {
			test.board = board;
			if (check_board(test, x, y)) {
				states.emplace_back(test);
			}
		}
	}

	return states;
}

int reversi::ai::predict_move()
{
	auto valid_states = get_next_states();
	for (int i = 0; i < valid_states.size(); i++) {
		std::cout << "Board" << std::endl;
		for (unsigned j = 0; j < REVERSI_BOARD_SIZE; j++) {
			if (j % 8 == 0) {
				std::cout << std::endl;
			}
			std::cout << " " << valid_states[i].board[j] << " ";
		}
		std::cout << std::endl;
	}
	return 26;
}

