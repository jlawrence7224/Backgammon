#pragma once
#include "eg_hash.h"

struct Roll;

constexpr int n_7_6 = 924;		// multichoose(7,6) == # of 6 checker inner board positions
constexpr int n_7_7 = 1716;		// multichoose(7,7) == # of 7 checker inner board positions
constexpr int n_7_8 = 3002;		// multichoose(7,8) == # of 8 checker inner board positions
constexpr int n_7_9 = 5005;		// multichoose(7,9) == # of 9 checker inner board positions

constexpr int n_checkers_x = 7;	// #checkers for which exact probabilities are calculated
constexpr int n_exact = n_7_7;  // #positions (max hash) for which exact P is calculated

/// <summary>
/// Exact cubeless win rate of late endgame positions
/// Both sides baring off with at most 7 checkers
/// A singleton class
/// </summary>
struct p_exact 
{
	using Hash = eg_hash::Hash;
	/// <summary>
	/// max # checkers of inner board configurations
	/// for which win probabilities are calculated.
	/// </summary>
	static const int n = n_checkers_x;
	/// <summary>
	/// max Hash value of 'n' checker configurations.
	/// </summary>
	static const Hash N = n_exact;
	using Pwin_t = std::array < std::array<float, N>, N>;

	Pwin_t p_win;

	p_exact() { init_p_exact(); }
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
	float Pwin(eg_hash::BwHash h) { return Pwin(h.first, h.second); }
};

// Singleton instance of p_exact
// Exact cubeless win rate of late endgame positions
// Both sides baring off with at most 7 checkers
extern p_exact exact;