// Rodrigo Custodio

#include <boost/asio/post.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/asio/thread_pool.hpp>



#include <algorithm>
// #include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#include <utility>


#include "ai.hpp"

#define FISRT_HEU_W 0.1F
#define SECOND_HEU_W 10.F
#define THIRD_HEU_W 70.F
#define FOURTH_HEU_W 10.F
#define FIFTH_HEU_W 500.F


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

void reversi::ai::print_board(const reversi::state &cstate)
{
	std::cout << "Board" << std::endl;
	for (unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		if (i % 8 == 0) {
			std::cout << std::endl;
		}
		std::cout << " " << cstate.board[i] << " ";
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
	int enemy = (tile == WHITE) ? BLACK : WHITE;

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

inline float reversi::ai::eval(const state &cstate)
{
	int enemy = (win_tile == WHITE) ? BLACK : WHITE;

	// First heuristic
	float simple_diff;
	// Second heuristic
	float owned_percentage = 0.;
	// Third heuristic
	float valid_moves_heu = 0.F;
	int valid_moves = count_valid_moves(cstate, win_tile);
	int enemy_valid_moves = count_valid_moves(cstate, enemy);
	if (win_tile == WHITE) {
		simple_diff = (float) (cstate.white_count - cstate.black_count);
		float sum = (float) (cstate.white_count + cstate.black_count);
		if (simple_diff < 0) {
			// I'm losing in this state
			owned_percentage = -(cstate.black_count) / sum * 100.F;
			valid_moves_heu = - enemy_valid_moves / sum * 100.F;
		} else {
			owned_percentage = (cstate.white_count) / sum * 100.F;
			valid_moves_heu = valid_moves / sum * 100.F;
		}
	} else {
		simple_diff = (float) (cstate.black_count - cstate.white_count);
		float sum = (float) (cstate.white_count + cstate.black_count);
		if (simple_diff < 0) {
			// I'm losing in this state
			owned_percentage = -(cstate.white_count) / sum * 100.F;
			valid_moves_heu = - enemy_valid_moves / sum * 100.F;
		} else {
			owned_percentage = (cstate.black_count) / sum * 100.F;
			valid_moves_heu = valid_moves / sum * 100.F;
		}
	}



	// Static weights acoording to Paul G.
	// @ An Analysis of Heuristics in Othello
	float static_weights[REVERSI_BOARD_SIZE] = {
		4.F, -3.F, 2.F, 2.F, 2.F, 2.F, -3.F, 4.F,
		-3.F, -4.F, -1.F, -1.F, -1.F, -1.F, -4.F, -3.F,
		2.F, -1.F, 1.F, 0.F, 0.F, 1.F, -1.F, 2.F,
		2.F, -1.F, 0.F, 1.F, 1.F, 0.F, -1.F, 2.F,
		2.F, -1.F, 0.F, 1.F, 1.F, 0.F, -1.F, 2.F,
		2.F, -1.F, 1.F, 0.F, 0.F, 1.0F, -1.F, 2.F,
		-3.F, -4.F, -1.F, -1.F, -1.F, -1.F, -4.F, -3.F,
		-4.F, -3.F, 2.F, 2.F, 2.F, 2.F -3.F, 4.F,
	};

	// Fourth heuristic
	float sum_board = 0.F;
	for (int i = 0; i < REVERSI_BOARD_SIZE; i++) {
		if (cstate.board[i] == win_tile) {
			sum_board += static_weights[i];
		} else if (cstate.board[i] == enemy) {
			sum_board += static_weights[i];
		}
	}

	// Fifth heuristic
	float corner_heu;
	int corners = 0;
	int corner_coords[4] = {0, 7, 56, 63};
	for (int i = 0; i < 4; i++) {
		int value = cstate.board[corner_coords[i]];
		if (value == win_tile) {
			corners += 1;
		} else if (value == enemy) {
			corners -= 1;
		}
	}
	corner_heu = 25.F * ((float) corners);

	return FISRT_HEU_W * simple_diff + SECOND_HEU_W * owned_percentage +
		THIRD_HEU_W * valid_moves_heu + FOURTH_HEU_W * sum_board +
		FIFTH_HEU_W * corner_heu;

}

std::vector<reversi::state>
reversi::ai::get_next_states(const reversi::state &cstate, int tile)
{
	std::vector<reversi::state> states;
	reversi::state test;
	for(int x = 0; x < REVERSI_ROW_LEN; x++) {
		for (int y = 0; y < REVERSI_COL_LEN; y++) {
			test = cstate; // Create a copy
			if (check_board(test, x, y, tile)) {
				test.pos = y * REVERSI_ROW_LEN + x;
				states.emplace_back(test);
			}
		}
	}

	return states;
}

int reversi::ai::count_valid_moves(const state &cstate, int tile)
{
	int count = 0;
	reversi::state test;
	for(int x = 0; x < REVERSI_ROW_LEN; x++) {
		for (int y = 0; y < REVERSI_COL_LEN; y++) {
			test = cstate; // Create a copy
			if (check_board(test, x, y, tile)) {
				count++;
			}
		}
	}
	return count;
}

float reversi::ai::minimaxab(const reversi::state &cstate, int depth,
			     float alpha, float beta, bool max, int tile)
{
	if (depth == 0 || cstate.white_count == 0 || cstate.black_count == 0 ||
	    REVERSI_BOARD_SIZE -
	    (cstate.black_count + cstate.white_count) == 0) {
		return eval(cstate);
	}

	// Generate next states
	// print_board(cstate);
	// std::cout << "MOVES " << tile << " " << win_tile << std::endl;
	int enemy = (tile == WHITE) ? BLACK : WHITE;
	auto valid_states = get_next_states(cstate, tile);
	// for (auto &state : valid_states) {
	// 	print_board(state);
	// }

	// No more childs
	if (valid_states.empty()) {
		return eval(cstate);
	}
	// Check node type
	if (max) {
		float max_value = -std::numeric_limits<float>::max();
		for (auto &child_state : valid_states) {
			float res = minimaxab(child_state, depth - 1, alpha,
					      beta, false, enemy);
			max_value = std::max(max_value, res);

			alpha = std::max(alpha, res);
			// Check for pruning
			if (beta <= alpha) {
				break;
			}
		}
		return max_value;
	} else {
		float min_value = std::numeric_limits<float>::max();
		for(auto &child_state : valid_states) {
			float res = minimaxab(child_state, depth - 1,
					    alpha, beta, true, enemy);
			min_value = std::min(min_value, res);
			beta = std::min(beta, res);
			if (beta <= alpha) {
				break;
			}

		}
		return min_value;
	}

}

void reversi::ai::minimax_worker(const state &cstate, int depth, float alpha,
				 float beta, bool max, int tile, int index)
{
	float res = minimaxab(cstate, depth, alpha,
			      beta, max, tile);
	{
		boost::lock_guard<boost::mutex> guard(max_mutex);
		if (res > last_max_value) {
			last_max_value = res;
			best_state_index = index;
		}
	}

}

boost::optional<reversi::state> reversi::ai::minimaxab_r()
{
	// First run is a max
	float beta = std::numeric_limits<float>::max();
	unsigned numthreads = std::thread::hardware_concurrency();
	boost::asio::thread_pool threadpool(numthreads);
	float alpha = -beta;
	int enemy = (win_tile == WHITE) ? BLACK : WHITE;
	auto valid_states = get_next_states(current_state, win_tile);
	last_max_value = alpha;
	best_state_index = -1;
	for (int i = 0; i < valid_states.size(); i++) {
		boost::asio::post(threadpool,
			boost::bind(&ai::minimax_worker, this, valid_states[i],
				    current_depth - 1, alpha,
				    beta, false, enemy, i));
	}
	// io_service.stop();
	threadpool.join();
	if (best_state_index == -1) {
		return boost::none;
	}
	return valid_states[best_state_index];
}

int reversi::ai::predict_move()
{
	// print_board();
	// std::cout << "MOVES" << std::endl;
	// auto valid_states = get_next_states(current_state, win_tile);
	// std::mt19937 rng(rd());
	// std::uniform_int_distribution<int> uni(0, valid_states.size());
	// auto random_integer = uni(rng);
	// return valid_states[random_integer].pos;
	// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	if (auto state = minimaxab_r()) {
		// std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
		// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() <<std::endl;
		return (*state).pos;
	}
	std::cout << "ERROR: Fatal error ocurred while predicting move"
		  << std::endl;
	return 0;
}

