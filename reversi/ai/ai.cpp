// Rodrigo Custodio

#include <algorithm>
#include <iostream>
#include <limits>
#include <utility>

#include "ai.hpp"


reversi::ai::ai(int depth)
	: current_depth(depth)
{
}

void reversi::ai::set_state(state &new_state)
{
	current_state = std::move(new_state);
}

void reversi::ai::set_tile(int tile)
{
	win_tile = tile;
}

void reversi::ai::set_depth(int new_depth)
{
	current_depth = new_depth;
}

void reversi::ai::print_board()
{
	std::cout << "Board" << std::endl;
	for (unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		if (i % 8 == 0) {
			std::cout << std::endl;
		}
		std::cout << " " << current_state.board[i] << " ";
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
			      int x, int y, int tile)
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
					if (tile == WHITE) {
						test.white_count++;
						test.black_count--;
					} else {
						test.white_count--;
						test.black_count++;
					}
					is_valid = true;
				}
			}
		}
	}

	if (tile == WHITE) {
		test.white_count++;
	} else {
		test.black_count++;
	}

	return is_valid;
}

inline int reversi::ai::eval(const state &cstate)
{
	if (win_tile == WHITE) {
		return cstate.white_count - cstate.black_count;
	} else {
		return cstate.black_count - cstate.white_count;
	}
}

std::vector<reversi::state>
reversi::ai::get_next_states(const reversi::state &cstate)
{
	std::vector<reversi::state> states;
	reversi::state test;
	for(int x = 0; x < REVERSI_ROW_LEN; x++) {
		for (int y = 0; y < REVERSI_COL_LEN; y++) {
			test = cstate; // Create a copy
			if (check_board(test, x, y, win_tile)) {
				test.pos = y * REVERSI_ROW_LEN + x;
				states.emplace_back(test);
			}
		}
	}

	return states;
}

int reversi::ai::minimaxab(const reversi::state &cstate, int depth, int alpha,
			   int beta, bool max)
{
	if (depth == 0 || cstate.white_count == 0 || cstate.black_count == 0 ||
	    REVERSI_BOARD_SIZE -
	    (cstate.black_count + cstate.white_count) == 0) {
		return eval(cstate);
	}

	// Generate next states
	auto valid_states = get_next_states(cstate);

	// No more childs
	if (valid_states.empty()) {
		return eval(cstate);
	}
	// Check node type
	if (max) {
		int max_value = std::numeric_limits<int>::min();
		for (auto &child_state : valid_states) {
			int res = minimaxab(child_state, depth - 1, alpha,
					    beta, false);
			max_value = std::max(max_value, res);
			alpha = std::max(alpha, res);
			// Check for pruning
			if (beta <= alpha) {
				break;
			}
			return max_value;
		}
	} else {
		int min_value = std::numeric_limits<int>::max();
		for(auto &child_state : valid_states) {
			int res = minimaxab(child_state, depth - 1,
					    alpha, beta, true);
			min_value = std::min(min_value, res);
			beta = std::min(beta, res);
			if (beta <= alpha) {
				break;
			}
			return min_value;
		}
	}
	// Should never happen
	return 0;
}

boost::optional<reversi::state> reversi::ai::minimaxab_r()
{
	// First run is a max
	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();
	int best_state_index = -1;
	auto valid_states = get_next_states(current_state);
	int max_value = alpha;
	for (int i = valid_states.size() - 1; i >= 0; i--) {
		int res = minimaxab(valid_states[i], current_depth - 1, alpha,
				    beta, false);
		if (res > max_value) {
			max_value = res;
			best_state_index = i;
		}
	}
	if (best_state_index == -1) {
		return boost::none;
	}
	return valid_states[best_state_index];
}

int reversi::ai::predict_move()
{
	// auto valid_states = get_next_states(current_state);
	// std::mt19937 rng(rd());
	// std::uniform_int_distribution<int> uni(0, valid_states.size());
	// auto random_integer = uni(rng);
	// return valid_states[random_integer].pos;
	if (auto state = minimaxab_r()) {
		return (*state).pos;
	}
	std::cout << "ERROR: Fatal error ocurred while predicting move"
		  << std::endl;
	return 0;
}

