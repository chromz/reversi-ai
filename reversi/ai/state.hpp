// Rodrigo Custodio

#ifndef REVERSI_STATE_H
#define REVERSI_STATE_H


#define REVERSI_BOARD_SIZE 64
#define REVERSI_ROW_LEN 8
#define REVERSI_COL_LEN 8

namespace reversi
{
	struct state
	{
		std::array<int, REVERSI_BOARD_SIZE> board;
		int points;
		int pos;
	};
}

#endif
