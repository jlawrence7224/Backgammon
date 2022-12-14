#pragma once
#include "eg_hash.h"

constexpr int n_7_6 = 924;		// multichoose(7,6) == # of 6 checker inner board positions
constexpr int n_7_7 = 1716;		// multichoose(7,7) == # of 7 checker inner board positions
constexpr int n_7_8 = 3002;		// multichoose(7,8) == # of 8 checker inner board positions
constexpr int n_7_9 = 5005;		// multichoose(7,9) == # of 9 checker inner board positions
constexpr int n_exact = 5005;	// n_7_8;

/// <summary>
/// Singleton class
/// Exact cubeless equity of late endgame positions
/// Both sides baring off with less than 6 checkers
/// </summary>
struct p_opt 
{
	using Hash = eg_hash::Hash;
	static const Hash N = n_exact;
	using Pwin_t = std::array < std::array<float, N>, N>;

	Pwin_t p_win;

	p_opt() { init_p_exact(); }
	// class initializers
	void  init_p_exact();
	void  init_p_exact(Hash hw, Hash hb, BoardInfo& b);
	float init_p_exact(int hb, BoardInfo& b, const Roll& r);

	/// <summary>
	/// The Win probability of player to move
	/// when both players use optimal strategy
	/// </summary>
	/// <param name="hw">inner board hash of player to move</param>
	/// <param name="hb">inner board hash of opponent</param>
	/// <returns>P(player to move wins)</returns>
	float Pwin(Hash toMove, Hash opp) { return p_win[toMove][opp]; }
};

extern p_opt exact;  // singleton