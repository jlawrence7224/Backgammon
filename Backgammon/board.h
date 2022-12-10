#pragma once
#include "inttyp.h"
#include <array>
#include <map>
#include <algorithm>
#include "roll.h"
#include "range.h"
#include "endgame.h"

class GameTree;

struct Info
{
	Bitboard occ_w;		// pips occupied by 1 or more white checkers
	Bitboard avail;		// points *not* occupied by 2 or more black checkers (+ bareoff bits)
	// Bitboard blot_b;	// points occupied by exactly 1 black checker
	uint outside;		// checkers outside of inner board
	bool bareOff() { return outside == 0; }	// bareOff moves available
};



struct Board {
	// From White perspective
	// White moves from bar 0 pips 1 to 24 finished 25
	// Black moves from 24 to 1
	static constexpr int16 init_pip_count = 2 * 24 + 5 * 13 + 3 * 8 + 5 * 6;
	Board()
		: _finishedB(0), _barB(0), pipCntW(init_pip_count), pipCntB(init_pip_count),
		board
	{ 0,	// bar White
		2,  0, 0, 0, 0, -5,
		0, -3, 0, 0, 0,  5,
	   -5,  0, 0, 0, 3,  0,
		5,  0, 0, 0, 0, -2,
		0,	// finished White
	}
	{};
	std::array<int8, 26> board;
	int8 _finishedB;
	int8 _barB;
	int16 pipCntW;
	int16 pipCntB;

	// Flip the board and initialize Info of the flipped board.
	Board(const Board& b, Info& info) : _finishedB(b.finished()), _barB(b.Wbar()), pipCntW(pipCntB), pipCntB(pipCntW)
	{
		board[0] = b._barB;
		int			inner = board[25] = b._finishedB;	// checkers in inner board or finished
		Bitboard	bit = Bitboard(2);	// Start with the second bit (first bit is White bar)
		Bitboard	occ_w = NO_SQUARES, pt_b = NO_SQUARES; // blot_b = NO_SQUARES

		for (int i = 1; i < 25; ++i)
		{
			int c = board[i] = -b.board[25 - i];
			if (c > 0)
			{
				occ_w |= bit;
				if (i > 18)
					inner += c;
			}
			else
			{
				pt_b |= (c < -1) ? bit : NO_SQUARES;
			}
			bit <<= 1;
		}

		info.occ_w = occ_w;
		// info.blot_b	= blot_b;
		info.outside = 15 - inner;
		info.avail = andn((info.outside < 4 && board[0] == 0) ? BAREOFF : BOARD, pt_b);
	}

	using InnerBoard = const endgame::inner_board&;
	// Initialize the inner board of White
	Board(const InnerBoard& b, Info& info) : _finishedB(0), _barB(0), pipCntW(0), pipCntB(0)
	{
		std::memset(&board[0], 0, sizeof board);
		board[0] = 0;
		board[18] = -15;	// put all black pieces with no contact
		board[25] = b[0];
		Bitboard occ_w = NO_SQUARES;
		for (int i = 1; i < 7; ++i)
		{
			if ((board[25 - i] = b[i]) > 0)
				occ_w |= (25 - i);
		}
		info.occ_w = occ_w;
		info.outside = 0;
		info.avail = BAREOFF;
	}

	int	Wbar()			const { return board[0]; }
	int Bbar()			const { return _barB; }
	int	finished()		const { return board[25]; }
	// ** TO DO ** Score backgammon
	float	Won() { return (finished() == 15) ? 1 + (_finishedB == 0) : 0; }

	void _hit(uint to)
	{
		Assert(board[to] == -1);
		board[to] = 1;
		++_barB;
	}
	int8 decr(Pip from) { return --board[from]; }
	void incr(Pip to, bool& hit)
	{
		uint to_cnt = board[to];
		if ((hit = (to_cnt < 0)))
			_hit(to);
		else
			board[to] = to_cnt + 1;
	}
	bool incr(Pip to)
	{
		bool hit;
		incr(to, hit);
		return hit;
	}
	void _undoHit(Pip to)
	{
		board[to] = -1;
		--_barB;
	}
	void undoMove(Pip from, Pip to, bool hit)
	{
		++board[from];
		if (hit) _undoHit(to); else --board[to];
	}

	void move(Pip from, Pip to, bool& hit)
	{
		Assert((to <= 25) && (to > from));
		decr(from);
		incr(to, hit);
	}
	// return hit rather than last
	bool move(uint from, uint to)
	{
		decr(from);
		return incr(to);
	}
	int8 bareOff(uint from)
	{
		Assert(from > 18);
		++board[25]; return --board[from];
	}
	void undoBareOff(uint from)
	{
		++board[from]; --board[25];
	}

	// occupied by White
	bool occupied(Pip p) { return board[p] > 0; }
	// available as destination of White move
	bool avail(Pip p) { return board[p] >= -1; }
	// No checkers on the pip
	bool empty(Pip p) { return board[p] == 0; }
	// A Black blot on the pip
	bool blot(Pip p) { return board[p] == -1; }

	bool operator<  (const Board& b) const
	{
		// Roughly, B1 < B2 means
		// white is less far along in B1 than B2
		// Every White move should result in a board such that
		// *board before move* < *board after move*
		if (pipCntW < b.pipCntW) return true;
		if (pipCntW > b.pipCntW) return false;
		if (pipCntB < b.pipCntB) return true;
		if (pipCntB > b.pipCntB) return false;
		return board > b.board;
	}

	// Are there any checkers on the inner board behind pip f (so it can bare off excess rolls)
	bool backmost(Pip f, Pip& to) {
		for (int i = 19; i < f; ++i)
			if (board[i] > 0)
				return false;
		return to = 25, true;
	}
	bool bareoffOK(Pip f, Pip& to, int outside) { Assert(to >= 25); return outside == 0 && (to == 25 || backmost(f, to)); }
	bool crossover(Pip f, Pip t) { return f < 19 && t >= 19; }

#if 0
	template<class MoveContainer = GameTree>
	void push_board(MoveContainer & tree);
	template<class MoveContainer = GameTree>
	void genMoves(MoveContainer& tree, Info& info, Die hi, Die lo);
	template<class MoveContainer = GameTree>
	void genMovesFromBar(MoveContainer& tree, Info& info, Die hi, Die lo);
	template<class MoveContainer = GameTree>
	void genMovesDoubles(MoveContainer& tree, Info& info, Die d, int n = 4);
	template<class MoveContainer = GameTree>
	void genMovesHiLo(MoveContainer& tree, Info& info, Die hi, Die lo);
	template<class MoveContainer = GameTree>
	void enqueMove(MoveContainer& tree, Pip from, Pip to);
	template<class MoveContainer = GameTree>
	void genOne(MoveContainer& tree, Bitboard w, Die d);
	template<class MoveContainer = GameTree>
	bool genN(MoveContainer& b, Bitboard occ, Bitboard avail, Die d, int outside, int n);
	template<class MoveContainer = GameTree>
	bool genHL(MoveContainer& tree, Bitboard occ, Bitboard a_hi, Bitboard a_lo, Die hi, Die lo, int outside);
#endif
	template<class MoveContainer>
	void push_board(MoveContainer& tree) { tree.push_board(*this); }


	template<class MoveContainer>
	void enqueMove(MoveContainer& tree, Pip from, Pip to)
	{
		bool hit = move(from, to);
		push_board(tree);
		undoMove(from, to, hit);
	}

	template<class MoveContainer>
	void genOne(MoveContainer& tree, Bitboard w, Die d)
	{
		for (auto from : w)
			enqueMove(tree, from, from + d);
	}

	template<class MoveContainer>
	void genMovesFromBar(MoveContainer& tree, Info& info, Die hi, Die lo)
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
	bool genN(MoveContainer& tree, Bitboard occ, Bitboard avail, Die d, int outside, int n)
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
		int cnt = std::min(n, int(board[from]));

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
	void genMovesDoubles(MoveContainer& tree, Info& info, Die d, int n = 4)
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
	bool genHL(MoveContainer& tree, Bitboard occ, Bitboard a_hi, Bitboard a_lo, Die hi, Die lo, int outside)
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
			if (move_lo && to_hi != to_lo)	// to_hi == to_lo if both rolls bareoff from f
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
	void genMovesHiLo(MoveContainer& tree, Info& info, Die hi, Die lo)
	{
		if (!genHL(tree, info.occ_w, info.avail >> hi, info.avail >> lo, hi, lo, info.outside))
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

	template<class MoveContainer>
	void genMoves(MoveContainer& tree, Info& info, Die hi, Die lo)
	{
		if (Wbar())
			genMovesFromBar(tree, info, hi, lo);
		else if (hi == lo)
			genMovesDoubles(tree, info, hi);
		else
			genMovesHiLo(tree, info, hi, lo);
	}

};


/// <summary>
/// A 'Board' augmented with 'Info' required for move generation
/// </summary>
struct BoardInfo : Board
{
	/// <summary>
	/// Flip the board 'b' and compute its move/position Info
	/// </summary>
	/// <param name="b"></param>
	BoardInfo(const Board& b) : Board(b, info) {}
	// Initialize the inner board of White
	BoardInfo(const InnerBoard& b) : Board(b,info) {}

	Info info;

	Info& get_Info() { return info; }
};

template<class MoveContainer>
void genMoves(MoveContainer& tree, BoardInfo& b, const Roll& r)
{
	b.genMoves(tree, b.get_Info(), r.hi, r.lo);
}


