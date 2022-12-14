#pragma once
#include "eg_hash.h"
#include "roll.h"

/// <summary>
/// A vector representation of a discrete probability density or distribution
/// such that P(X=n) == 0.0 for n < lower() or n >= upper()
/// thus has finite support.
/// </summary>
struct finite_support_vector
{
	finite_support_vector(size_t upper, size_t lower) : support(upper - lower), offset(lower) {}
	finite_support_vector(float f) : support(1), offset(0) { support[0] = f; }
	std::vector<float> support;
	size_t  offset;

	/// <summary>
	/// upper bound of non-zero values
	/// </summary>
	/// <returns></returns>
	size_t  upper() { return support.size() + offset; }
	/// <summary>
	/// lower bound of non-zero values
	/// </summary>
	/// <returns></returns>
	size_t  lower() { return offset; }

	float   operator[](size_t i) { return support[i - offset]; }
	float   at(int i) { return (i < lower()) ? 0.0 : (i >= upper()) ? 1.0 : (*this)[i]; }

	// Increment offset by 1 to shift the domain of the distribution
	size_t  shift() { return ++offset; }
	// Convert density to distribution
	void    distribution()
	{
		double d = 0.0;
		for (int i = 0; i < support.size(); ++i) {
			d += support[i];
			support[i] = d;
		}
		Assert(std::abs(d - 1.0) < 1.0e-6);
		support[support.size() - 1] = 1.0;
	}
};

/// <summary>
/// for each inner table position
/// The Expectation of the
/// random variable X where X=n is the event that a player
/// bares off in n turns when always choosing min ENR moves.
/// </summary>
struct ENR {
	using enrvec = std::vector<float>;
	using Hash = eg_hash::Hash;
	enrvec enr;

	ENR() { compute_enr(); }

	// Initializers
	void    compute_enr();
	float   compute_enr(BoardInfo& b, const Roll& r);
	float   compute_enr(BoardInfo& b);

	/// <summary>
	/// Hash of best move using min ENR strategy
	/// </summary>
	/// <param name="b"></param>
	/// <param name="r"></param>
	/// <returns></returns>
	Hash    bestMove(BoardInfo& b, const Roll& r) const;

	/// <summary>
	/// E[X]
	/// Expected value of X for inner table position with hash h
	/// </summary>
	/// <param name="h"></param>
	/// <returns></returns>
	float	operator[](Hash h) const { return enr[h]; }
};

/// <summary>
/// for each inner table position
/// The Expectation and Probability distribution of the
/// random variable X where X=n is the event that a player
/// bares off in n turns when always choosing min ENR moves.
/// </summary>
struct PNR : ENR
{
	using Dist = finite_support_vector;
	using Hash = ENR::Hash;

	std::vector<Dist> X;

	PNR() : ENR() { computeXdist(); }

	// Initializers
	void computeXdist();
	void computeXden();
	void computeXden(BoardInfo& b);

	/// <summary>
	/// The Win probability of player to move
	/// when both players use min ENR strategy
	/// </summary>
	/// <param name="hw">inner board hash of player to move</param>
	/// <param name="hb">inner board hash of opponent</param>
	/// <returns>P(player to move wins)</returns>
	float Pwin(Hash hw, Hash hb);
};

/// <summary>
/// for each inner table position
/// The Expectation and Probability distribution of the
/// random variable X where X=n is the event that a player
/// bares off in n turns when always choosing min ENR moves.
/// 
/// The singleton instance of struct PNR;
/// </summary>
extern struct PNR Pnr;
