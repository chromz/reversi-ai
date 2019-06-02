// Rodrigo Custodio

#ifndef REVERSI_AI_H
#define REVERSI_AI_H

#include <array>
#include <boost/optional.hpp>
#include <vector>
#include "state.hpp"

#define BLACK 2
#define WHITE 1

namespace reversi
{
	class ai
	{
	public:
		ai(int depth);
		void set_state(state &new_state);
		void set_tile(int tile);
		void set_depth(int new_depth);
		void print_board();
		int predict_move();
	private:

		state current_state;
		int current_depth;

		int win_tile;

		int eval(const state &cstate);
		int pos(const std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y);
		void set(std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y, int val);
		bool is_inside(int x, int y);
		bool check_board(state &test, int x, int y, int tile);
		std::vector<state> get_next_states(const state &cstate);
		int minimaxab(const state &cstate, int depth, int alpha,
			      int beta, bool max);

		// Re-entrant version of minimaxab
		boost::optional<state>  minimaxab_r();

	};
}

#endif
