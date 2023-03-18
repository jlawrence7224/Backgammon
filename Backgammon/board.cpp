
#include <iostream>
#include <stdlib.h>
#include "board.h"
#include "gametree.h"
#include "eg_hash.h"


/// <summary>
/// Initialize board to an endgame bare-off race
/// from the hashes of white and black inner boards
/// </summary>
/// <param name="w">hash of White's inner board</param>
/// <param name="b">hash of Black's inner board</param>
Board::Board(int64 w, int64 b)
{
	inner_table wt, bt;
	eg.inverse_hash(w, wt);
	eg.inverse_hash(b, bt);
	*this = Board(wt, bt);
}

std::ostream& operator<<(std::ostream& s, const Board& b)
{
	s << std::endl;

	if (b.finishedW() || b.finishedB())
		s << std::endl << "Finished W: " << b.finishedW() << " B: " << b.finishedB() << std::endl;

	s << std::endl << " 24  23  22  21  20  19  BAR  18  17  16  15  14  13";
	s << std::endl << "|--- --- --- --- --- ---|---|--- --- --- --- --- ---|";
	for (int i = 1; i <= 5; ++i)
	{
		s << std::endl << "|";
		for (int j = 24; j > 18; --j)
		{
			if (int cnt = std::abs(b.board[j]))
			{
				if (cnt >= i)
				{
					if (i < 5)
					{
						s << (b.board[j] > 0 ? " * " : " X ");
					}
					else
					{
						if (cnt < 10)
							s << " ";
						s << cnt << " ";
					}
				}
				else
					s << "   ";
			}
			else
				s << "   ";
			if (j > 19)
				s << " ";
		}
		s << ((b.Wbar() >= i) ? "| * |" : "|   |");

		for (int j = 18; j > 12; --j)
		{
			if (int cnt = std::abs(b.board[j]))
			{
				if (cnt >= i)
				{
					if (i < 5)
					{
						s << (b.board[j] > 0 ? " * " : " X ");
					}
					else
					{
						if (cnt < 10)
							s << " ";
						s << cnt << " ";
					}
				}
				else
					s << "   ";
			}
			else
				s << "   ";
			if (j > 13)
				s << " ";
		}
		s << "|";
	}
	s << std::endl << "|                       |   |                       |";
	s << std::endl << "|--- --- --- --- --- ---|---|--- --- --- --- --- ---|";
	s << std::endl << "|                       |   |                       |";
	for (int i = 5; i >= 1; --i)
	{
		s << std::endl << "|";
		for (int j = 1; j < 7; ++j)
		{
			if (int cnt = std::abs(b.board[j]))
			{
				if (cnt >= i)
				{
					if (i < 5)
					{
						s << (b.board[j] > 0 ? " * " : " X ");
					}
					else
					{
						if (cnt < 10)
							s << " ";
						s << cnt << " ";
					}
				}
				else
					s << "   ";
			}
			else
				s << "   ";
			if (j < 6)
				s << " ";
		}
		s << ((b.Bbar() >= i) ? "| X |" : "|   |");

		for (int j = 7; j < 13; ++j)
		{
			if (int cnt = std::abs(b.board[j]))
			{
				if (cnt >= i)
				{
					if (i < 5)
					{
						s << (b.board[j] > 0 ? " * " : " X ");
					}
					else
					{
						if (cnt < 10)
							s << " ";
						s << cnt << " ";
					}
				}
				else
					s << "   ";
			}
			else
				s << "   ";
			if (j < 12)
				s << " ";
		}
		s << "|";
	}
	s << std::endl << "|--- --- --- --- --- ---|---|--- --- --- --- --- ---|";
	s << std::endl << "  1   2   3   4   5   6  BAR  7   8   9   10  11  12";
	s << std::endl;
	return s;
}
