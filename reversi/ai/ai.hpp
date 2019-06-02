// Rodrigo Custodio

#ifndef REVERSI_AI_H
#define REVERSI_AI_H

#include <array>
#include <vector>
#include "state.hpp"

namespace reversi
{
	class ai
	{
	public:
		ai();
		void set_board(std::array<int, REVERSI_BOARD_SIZE> board);
		void set_tile(int tile);
		void print_board();
		int predict_move();
	private:
		std::array<int, REVERSI_BOARD_SIZE> board;
		int tile;

		int pos(const std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y);
		void set(std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y, int val);
		bool is_inside(int x, int y);
		bool check_board(state &test, int x, int y);
		std::vector<state> get_next_states();

	};
}

#endif
