
#include <iostream>
#include <stdlib.h>
#include "board.h"
#include "gametree.h"


std::ostream& operator<<(std::ostream& s, const Board& b)
{
	s << std::endl << "  24  23  22  21  20  19 BAR  18  17  16  15  14  13";
	s << std::endl << "|--- --- --- --- --- ---|---|--- --- --- --- --- --- |";
	for (int i = 1; i <= 5; ++i)
	{
		s << std::endl << "| ";
		for (int j = 24; j > 18; --j)
		{
			int cnt = std::abs(b.board[j]);

			if (cnt >= i)
			{
				if (i <= 5)
				{
					s << (b.board[j] > 0 ? "*   " : "X   ");
				}
				else
				{
					s << cnt;
					s << (cnt < 10 ? "   " : "  ");
				}
			}
			else
				s << "    ";
		}
		s << ((b.Wbar() >= i) ? "*   " : "    ");

		for (int j = 18; j > 12; --j)
		{
			int cnt = std::abs(b.board[j]);

			if (cnt >= i)
			{
				if (i <= 5)
				{
					s << (b.board[j] > 0 ? "*   " : "X   ");
				}
				else
				{
					s << cnt;
					s << (cnt < 10 ? "   " : "  ");
				}
			}
			else
				s << "    ";
		}
	}
	s << std::endl << "|                       |   |                        |";
	s << std::endl << "|--- --- --- --- --- ---|---|--- --- --- --- --- --- |";
	s << std::endl << "|                       |   |                        |";
	for (int i = 5; i <= 1; ++i)
	{
		s << std::endl << "| ";
		for (int j = 1; j <= 6; ++j)
		{
			int cnt = std::abs(b.board[j]);

			if (cnt >= i)
			{
				if (i <= 5)
				{
					s << (b.board[j] > 0 ? "*   " : "X   ");
				}
				else
				{
					s << cnt;
					s << (cnt < 10 ? "   " : "  ");
				}
			}
			else
				s << "    ";
		}
		s << ((b.Bbar() >= i) ? "X   " : "    ");

		for (int j = 7; j <= 12; ++j)
		{
			int cnt = std::abs(b.board[j]);

			if (cnt >= i)
			{
				if (i <= 5)
				{
					s << (b.board[j] > 0 ? "*   " : "X   ");
				}
				else
				{
					s << cnt;
					s << (cnt < 10 ? "   " : "  ");
				}
			}
			else
				s << "    ";
		}
	}
	return s;
}
#if 0
template<class MoveContainer>
void Board::push_board(MoveContainer & tree) { tree.push_board(*this); }

template<class MoveContainer>
void Board::genMoves(MoveContainer& tree, Info& info, Die hi, Die lo)
{
	if (Wbar())
		genMovesFromBar(tree, info, hi, lo);
	else if (hi == lo)
		genMovesDoubles(tree, info, hi);
	else
		genMovesHiLo(tree, info, hi, lo);
}

template<class MoveContainer>
void Board::enqueMove(MoveContainer& tree, Pip from, Pip to)
{
	bool hit = move(from, to);
	push_board(tree);
	undoMove(from, to, hit);
}

template<class MoveContainer>
void Board::genOne(MoveContainer& tree, Bitboard w, Die d)
{
	for (auto from : w)
		enqueMove(tree, from, from + d);
}

template<class MoveContainer>
void Board::genMovesFromBar(MoveContainer& tree, Info& info, Die hi, Die lo)
{
	Assert(Wbar());
	int to_hi = hi;
	int to_lo = lo;
	Bitboard occ_w = info.occ_w;
	Bitboard avail = info.avail;	// pips not occupied by two or more black checkers

	bool a_hi = member(avail, to_hi), a_lo = member(avail, to_lo);
	if (!(a_hi || a_lo))
	{
		// can not move
		push_board(tree);
		return; 	// Null move
	}

	bool hit_hi, hit_lo;

	if (hi != lo)
	{
		if (Wbar() > 1)	// more than one checker on the bar
		{
			// Only possible move is bar\hi bar\lo
			if (a_hi)
				move(w_bar, to_hi, hit_hi);
			if (a_lo)
				move(w_bar, to_lo, hit_lo);
			push_board(tree);
			if (a_hi)
				undoMove(w_bar, to_hi, hit_hi);
			if (a_lo)
				undoMove(w_bar, to_lo, hit_lo);
			return;
		}
		else // one checker on the bar
		{
			bool both_moves_taken = false;

			if (a_hi)
			{
				if (Bitboard w_lo = (occ_w | hi) & (avail >> lo))	// pips from which w can move the lo roll
				{
					move(w_bar, to_hi, hit_hi);
					genOne(tree, w_lo, lo);
					undoMove(w_bar, to_hi, hit_hi);
					both_moves_taken = true;
					if (member(avail, hi + lo) && board[lo] >= 0 && !hit_hi)
					{
						// If we generated a bar/hi/hi+lo move 
						// and neither board[hi] nor board[lo] are hits
						// Then don't generate a bar/lo/hi+lo move

						lo = 31;	// Set lo to a value outside of avail >> hi
					}
				}
			}

			if (a_lo)
			{
				if (Bitboard w_hi = (occ_w | lo) & (avail >> hi))	// pips from which w can move the hi roll)
				{
					move(w_bar, to_lo, hit_lo);
					genOne(tree, w_hi, hi);
					undoMove(w_bar, to_lo, hit_lo);
					both_moves_taken = true;
				}
			}

			if (!both_moves_taken)
			{
				// unable to take both moves
				if (a_hi)
				{
					move(w_bar, to_hi, hit_hi);
					push_board(tree);
					undoMove(w_bar, to_hi, hit_hi);
				}
				if (a_lo)
				{
					move(w_bar, to_lo, hit_lo);
					push_board(tree);
					undoMove(w_bar, to_lo, hit_lo);
				}
			}
		}
	}
	else // hi==lo -- rolled doubles
	{
		Assert(a_hi);
		int checkers_on_bar = Wbar();
		int moves_from_bar = std::max(4, checkers_on_bar);

		// bare on up to 4 checkers
		move(w_bar, to_hi, hit_hi);		// The first bareOn may be a hit
		for (int i = 1; i < moves_from_bar; ++i)
			move(w_bar, to_hi);

		Bitboard w_hi = (occ_w | to_hi) & (avail << hi);	// pips from which w can move the hi roll
		int moves_remaining = 4 - moves_from_bar;
		genMovesDoubles(tree, info, hi, moves_remaining);

		// Undo the bare on's
		for (int i = 1; i < moves_from_bar; ++i)
			undoMove(w_bar, to_hi, false);
		undoMove(w_bar, to_hi, hit_hi);
	}
}

template<class MoveContainer>
bool Board::genN(MoveContainer& tree, Bitboard occ, Bitboard avail, Die d, int outside, int n)
{
	if (n == 0)
	{
		push_board(tree);
		return true;
	}
	if (!occ)
	{
		if (finished() == 15)
		{
			push_board(tree);
			return true;
		}
		else
			return false;
	}

	int from = *occ;
	int to = from + d;
	int cnt = std::max(n, int(board[from]));

	// Bare off?
	if (to > 24)
	{
		if (outside == 0)
		{
			if (to > 25)
			{
				for (int i = 19; i < from; ++i)
					if (board[i] > 0)
						return false;
			}
			for (int i = 0; i < cnt; ++i)
				bareOff(from);
			bool ret = genN(tree, ++occ, avail, d, 0, n - cnt);
			for (int i = 0; i < cnt; ++i)
				undoBareOff(from);
			return ret;
		}
		Assert(outside > 0);
		return false;
	}

	// update the occupied sets
	++occ;
	Bitboard occN = occ | (Bit(to) & avail);


	// make cnt from/to moves (recording hit)
	bool hit = move(from, to);
	for (int i = 1; i < cnt; ++i)
		move(from, to);

	// update the count of checkers outside of inner table
	int moved_in = (to > 18) && (from <= 18) ? cnt : 0;

	// try generating moves if we play 
	// cnt checkers from this pip
	bool ret = genN(tree, occN, avail, d, outside - moved_in, n - cnt);
	bool more = ret;

	while (--cnt > 0)
	{
		// Try generating moves if we play
		// cnt-1 .. 1 checkers from this pip.
		undoMove(from, to, false);
		if (more)
			more = genN(tree, occN, avail, d, outside - (moved_in ? cnt : 0), n - cnt);
	}
	undoMove(from, to, hit);
	if (more)
		genN(tree, occ, avail, d, outside, n);
	return ret;
}

template<class MoveContainer>
void Board::genMovesDoubles(MoveContainer& tree, Info& info, Die d, int n)
{
	// Try to generate n moves from this position, 
	// if that fails (genN returns false) try (n-1)..0 until success
	// genN returns true when n == 0
	while (!genN(tree, info.occ_w, info.avail - d, d, info.outside, n))
	{
		Assert(n > 0);
		--n;
	}
}

template<class MoveContainer>
bool Board::genHL(MoveContainer& tree, Bitboard occ, Bitboard a_hi, Bitboard a_lo, Die hi, Die lo, int outside)
{
	Assert(finished() < 15);

	bool ret = false;
	while (occ)
	{
		Pip f = *occ; ++occ;	// iterate over occupied Pips

		Pip to_hi = f + hi, to_lo = f + lo;
		bool move_hi = member(a_hi, f) && ((to_hi < 25) || bareoffOK(f, to_hi, outside));
		bool move_lo = member(a_lo, f) && ((to_lo < 25) || bareoffOK(f, to_lo, outside));

		if (move_hi)
		{
			bool hit_hi = move(f, to_hi);
			if (move_lo && board[f] > 0)
			{
				enqueMove(tree, f, to_lo);
				ret = true;
			}

			// complete the move taking the lo roll from a pip further down the board
			ret |= genN(tree, occ + to_hi, a_lo, lo, outside - crossover(f, to_hi), 1);
			undoMove(f, to_hi, hit_hi);
		}
		if (move_lo && to_hi != to_lo )	// to_hi == to_lo if both rolls bareoff from f
		{
			bool hit_lo = move(f, to_lo);
			// Avoid duplicate moves: f/f+lo+hi == f/f+hi/f+lo+hi == f/f+lo/f+lo+hi
			Bitboard avail_hi = (move_hi && !(hit_lo || blot(to_hi))) ? a_hi - to_lo : a_hi;

			// complete the move taking the hi roll from a pip further down the board
			ret |= genN(tree, occ + to_lo, avail_hi, hi, outside - crossover(f, to_lo), 1);
			undoMove(f, to_lo, hit_lo);
		}
	}
	return ret;
}

template<class MoveContainer>
void Board::genMovesHiLo(MoveContainer& tree, Info& info, Die hi, Die lo)
{
	if( !genHL( tree, info.occ_w, info.avail >> hi, info.avail >> lo, hi, lo, info.outside) )
	{ 
		// Can not play both hi and lo rolls. Try playing just one of the die.
		bool play_hi = genN(tree, info.occ_w, info.avail >> hi, hi, info.outside, 1);
		bool play_lo = genN(tree, info.occ_w, info.avail >> lo, lo, info.outside, 1);
		if (!(play_hi || play_lo))
		{
			// can not play any move
			push_board(tree);	// null move
		}
	}
}

#endif