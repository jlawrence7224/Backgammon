#include "scm.h"
#include "board.h"
#include "eval.h"

struct eg_hash eg;  // singleton
PNR Pnr;
p_exact exact;  // singleton

bool Board::bareoffB() const
{
	// Are all Black checkers finished or in the inner board
	int cnt = finishedB();
	for (int i = 1; i <= 6; ++i)
			cnt -= std::min(0,int(board[i]));
	return cnt == 15;
}

bool Board::bareoffW() const
{
	// Are all White checkers finished or in the inner board
	int cnt = finishedW();
	for (int i = 19; i <= 24; ++i)
			cnt += std::max(0,int(board[i]));
	return cnt == 15;
}

bool Board::bareoff_race()		const
{
	// Use pip count to filter pointless bareoff tests.
	return pipB() <= 6 * 15 && pipW() <= 6 * 15
		&& bareoffB() && bareoffW();
}

float Board::endgame_eval(bool& terminal) const
{
	const auto min_finished = 15 - p_exact::n;
	auto h = eg.hash_bw(*this);
	return (finishedW() >= min_finished && finishedB() >= min_finished)
		? terminal=true, exact.Pwin(h) : terminal=false, Pnr.Pwin(h);
}

float Board::eval(bool& terminal)		const
{
	return bareoff_race() ? endgame_eval(terminal) : terminal=false, ScmPwin(pipW(), pipB());
}