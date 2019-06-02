// Rodrigo Custodio

#include <array>
#include <iostream>
#include <functional>
#include <vector>

#include "game.hpp"

#define TREE_DEPTH 100000


reversi::game::game(const std::string &host, const int port)
	: host(host),
	  port(port),
	  tourid(0),
	  reversi_ai(TREE_DEPTH)
{
}

void reversi::game::on_connect()
{
	std::cout << "Connection established" << std::endl;
	sio::message::ptr msg = sio::object_message::create();
	msg->get_map()["user_name"] = sio::string_message::create(name);
	msg->get_map()["tournament_id"] = sio::int_message::create(tourid);
	msg->get_map()["user_role"] = sio::string_message::create(
			std::string("player"));
	std::cout << "Registering user" << std::endl;
	handler.socket()->emit("signin", msg);
}

void reversi::game::on_disconnect(sio::client::close_reason const &reason)
{
	std::cout << "Disconnected" << std::endl;
}

void reversi::game::on_failure()
{
	std::cout << "Connection failure" << std::endl;
}

void reversi::game::on_ok_signin(sio::event &event)
{
	std::cout << "Registration completed" << std::endl;
}


void reversi::game::on_ready(std::string const &name,
			sio::message::ptr const &data,
			bool is_ack, sio::message::list &ack_resp)
{
	auto game_id = data->get_map()["game_id"];
	auto player_id = data->get_map()["player_turn_id"];
	std::vector<sio::message::ptr> board_msg = data->get_map()["board"]->
		get_vector();
	reversi::state state;
	std::array<int, REVERSI_BOARD_SIZE> board;
	for(unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		state.board[i] = board_msg[i]->get_int();
		if (state.board[i] == WHITE) {
			state.white_count++;
		}

		if (state.board[i] == BLACK) {
			state.black_count++;
		}
	}
	reversi_ai.set_state(state);
	reversi_ai.set_tile(player_id->get_int());
	// reversi_ai.print_board(); // Remember to remove
	int movement = reversi_ai.predict_move();
	sio::message::ptr msg = sio::object_message::create();
	msg->get_map()["tournament_id"] = sio::int_message::create(tourid);
	msg->get_map()["player_turn_id"] = player_id;
	msg->get_map()["game_id"] = game_id;
	msg->get_map()["movement"] = sio::int_message::create(movement);
	handler.socket()->emit("play", msg);
}

void reversi::game::on_finish(std::string const &name,
			sio::message::ptr const &data,
			bool is_ack, sio::message::list &ack_resp)
{
	auto game_id = data->get_map()["game_id"];
	auto player_id = data->get_map()["player_turn_id"];
	std::vector<sio::message::ptr> board_msg = data->get_map()["board"]->
		get_vector();
	reversi::state state;
	std::array<int, REVERSI_BOARD_SIZE> board;
	for(unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		state.board[i] = board_msg[i]->get_int();
		if (state.board[i] == WHITE) {
			state.white_count++;
		}

		if (state.board[i] == BLACK) {
			state.black_count++;
		}
	}
	reversi_ai.set_state(state);
	std::cout << "FINISH" << std::endl;
	// reversi_ai.print_board();
	sio::message::ptr msg= sio::object_message::create();
	msg->get_map()["tournament_id"] = sio::int_message::create(tourid);
	msg->get_map()["player_turn_id"] = player_id;
	msg->get_map()["game_id"] = game_id;
	handler.socket()->emit("player_ready", msg);
}

void reversi::game::start(const std::string &name, const int tourid)
{
	this->name = name;
	this->tourid = tourid;
	handler.set_open_listener(std::bind(&game::on_connect, this));
	handler.set_close_listener(std::bind(&game::on_disconnect, this,
				   std::placeholders::_1));
	handler.set_fail_listener(std::bind(&game::on_failure, this));

	auto aux_on_ready = sio::socket::event_listener_aux(
		std::bind(&game::on_ready, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4));
	auto aux_on_finish = sio::socket::event_listener_aux(
		std::bind(&game::on_finish, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4));
	// Connect to the server
	handler.connect("http://" + host + ":" + std::to_string(port));
	handler.socket()->on("ok_signin",
		std::bind(&game::on_ok_signin, this, std::placeholders::_1));
	handler.socket()->on("ready", aux_on_ready);
	handler.socket()->on("finish", aux_on_finish);
	// Wait until threads notify to stop waiting;
	{
		std::unique_lock<std::mutex> lock(mtx);
		condition.wait(lock, [&](){ return stop; });
	}
	handler.close();
}
