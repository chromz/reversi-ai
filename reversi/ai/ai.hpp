// Rodrigo Custodio

#ifndef REVERSI_AI_H
#define REVERSI_AI_H

#define REVERSI_BOARD_SIZE 64

namespace reversi
{
	class ai
	{
	public:
		ai();
		void set_board(int *board);
		void print_board();
		int predict_move();
	private:
		int *board;
	};
}

#endif
