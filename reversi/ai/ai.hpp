// Rodrigo Custodio

#ifndef REVERSI_AI_H
#define REVERSI_AI_H

#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include <array>
#include <random>
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
		std::random_device rd;
		boost::mutex max_mutex;
		int best_state_index;
		float last_max_value;

		int win_tile;

		float eval(const state &cstate);
		int pos(const std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y);
		void print_board(const state &cstate);
		void set(std::array<int, REVERSI_BOARD_SIZE> &board,
			int x, int y, int val);
		bool is_inside(int x, int y);
		bool check_board(state &test, int x, int y, int tile);
		std::vector<state> get_next_states(const state &cstate,
						   int tile);
		int count_valid_moves(const state &cstate, int tile);
		float minimaxab(const state &cstate, int depth, float alpha,
				float beta, bool max, int tile);

		void minimax_worker(const state &cstate, int depth, float alpha,
				    float beta, bool max, int tile, int index);

		// Re-entrant version of minimaxab
		boost::optional<state>  minimaxab_r();

	};
}

#endif
