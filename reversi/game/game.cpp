// Rodrigo Custodio


#include <iostream>
#include <functional>
#include <vector>

#include "game.hpp"

reversi::game::game(const std::string &host, const int port)
	: host(host),
	  port(port),
	  tourid(0)
{
}

void reversi::game::print_board()
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
	std::cout << "Ready" << std::endl;
	auto game_id = data->get_map()["game_id"];
	auto player_id = data->get_map()["player_turn_id"];
	std::vector<sio::message::ptr> board_msg = data->get_map()["board"]->
		get_vector();
	for(unsigned i = 0; i < REVERSI_BOARD_SIZE; i++) {
		board[i] = board_msg[i]->get_int();
	}
	print_board();
	std::cout << "Enter value:" << std::endl;
	int movement;
	std::cin >> movement;
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
	// Game mainloop temporal solution
	while (true) {}
}
