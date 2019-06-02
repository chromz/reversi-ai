// Rodrigo Custodio
#ifndef REVERSI_PLAYER_H
#define REVERSI_PLAYER_H

#include <condition_variable>
#include <mutex>
#include <sio_client.h>
#include <string>

#include "ai/ai.hpp"


namespace reversi
{
	class game
	{
	public:
		game(const std::string &host, int port);
		void start(const std::string &name, const int tourid);

	private:
		const std::string host;
		const int port;
		std::mutex mtx;
		std::condition_variable condition;
		ai reversi_ai;
		bool stop = false;
		sio::client handler;
		std::string name;
		int tourid;

		void on_connect();
		void on_disconnect(
				sio::client::close_reason const& reason);
		void on_failure();
		void on_ok_signin(sio::event &event);
		void on_ready(std::string const &name,
			sio::message::ptr const &data,
			bool is_ack, sio::message::list &ack_resp);
		void on_finish(std::string const &name,
			sio::message::ptr const &data,
			bool is_ack, sio::message::list &ack_resp);
	};
}

#endif
