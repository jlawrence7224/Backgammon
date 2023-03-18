#include <array>
#include <algorithm>
#include "scm.h"

constexpr float INF = std::numeric_limits<float>::infinity();
/// <summary>
/// Single Checker Model of Backgammon races 
/// </summary>
struct Scm {

	// Distribution of the D^2/S statistic
#if 0
	  0.50, 0.00000000,
	  0.51, 0.00137882,
	  0.52, 0.00551875,
	  0.53, 0.0124302,
	  0.54, 0.0221307,
	  0.55, 0.0346450,
	  0.56, 0.0500050,
	  0.57, 0.0682506,
	  0.58, 0.0894296,
	  0.59, 0.113598,
	  0.60, 0.140821,
	  0.61, 0.171174,
	  0.62, 0.204741,
	  0.63, 0.241618,
	  0.64, 0.281913,
	  0.65, 0.325747,
	  0.66, 0.373256,
	  0.67, 0.424591,
	  0.68, 0.479920,
	  0.69, 0.539433,
	  0.70, 0.603341,
	  0.71, 0.671879,
	  0.72, 0.745311,
	  0.73, 0.823934,
	  0.74, 0.908082,
	  0.75, 0.998131,
	  0.76, 1.09451,
	  0.77, 1.19769,
	  0.78, 1.30824,
	  0.79, 1.42679,
	  0.80, 1.55407,
	  0.81, 1.69092,
	  0.82, 1.83834,
	  0.83, 1.99749,
	  0.84, 2.16975,
	  0.85, 2.35678,
	  0.86, 2.56060,
	  0.87, 2.78365,
	  0.88, 3.02902,
	  0.89, 3.30059,
	  0.90, 3.60337,
	  0.91, 3.94399,
	  0.92, 4.33145,
	  0.93, 4.77844,
	  0.94, 5.30360,
	  0.95, 5.93596,
	  0.96, 6.72439,
	  0.97, 7.76102,
	  0.98, 9.25404,
	  0.99, 11.8737,
#endif

	const std::array<float, 50> _Y = {
		  0.00000000,
		  0.00137882,
		  0.00551875,
		  0.0124302,
		  0.0221307,
		  0.0346450,
		  0.0500050,
		  0.0682506,
		  0.0894296,
		  0.113598,
		  0.140821,
		  0.171174,
		  0.204741,
		  0.241618,
		  0.281913,
		  0.325747,
		  0.373256,
		  0.424591,
		  0.479920,
		  0.539433,
		  0.603341,
		  0.671879,
		  0.745311,
		  0.823934,
		  0.908082,
		  0.998131,
		  1.09451,
		  1.19769,
		  1.30824,
		  1.42679,
		  1.55407,
		  1.69092,
		  1.83834,
		  1.99749,
		  2.16975,
		  2.35678,
		  2.56060,
		  2.78365,
		  3.02902,
		  3.30059,
		  3.60337,
		  3.94399,
		  4.33145,
		  4.77844,
		  5.30360,
		  5.93596,
		  6.72439,
		  7.76102,
		  9.25404,
		  11.8737,
	};

	using it = decltype(_Y)::const_iterator;

	// First and second divided difference of inverse relation
	std::array<float, 50> D1;
	std::array<float, 50> D2;

	float Y(int i)	{ return _Y[i]; }
	float Y(it i)	{ return *i; }
	float X(int i)	{ return i + 50; }

	// Initialize divided differences of the inverse relation: Y->X
	void initDividedDifference()
	{
		D1[0] = D2[0] = D2[1] = 0.0;
		for (int i = 1; i < 50; ++i)
			D1[i] = 1.0 / (Y(i) - Y(i - 1));
		for (int i = 2; i < 50; ++i)
			D2[i] = (D1[i] - D1[i - 1]) / (Y(i) - Y(i - 1));
	}

	// Newton Quadratic Interpolation

	// Interpolation from Nth ordinate
	float NQI(float y, int n)
	{
		return X(n) + (y - Y(n)) * D1[n + 1] + (y - Y(n)) * (y - Y(n + 1)) * D2[n + 2];
	}
	/// <summary>
	/// Newton Quadratic Interpolation
	/// of single checker model win probability distribution.
	/// </summary>
	/// <param name="y"></param>
	/// <returns></returns>
	float NQI(float y)
	{
		Assert(f >= 0.0);
		int n = std::lower_bound(_Y.begin(), _Y.end(), y) - _Y.begin();
		return NQI(y, n)/100.0;
	}

	// Aitken's iterative interpolation

	// Table of ordinates and value of p_0..n(y) -- Nth degree interpolating polynomial
	using Tab = std::vector<std::pair<float,float> >;

	float del(Tab& T, int a, int b)
	{
		double Ax = T[a].first, Ay = T[a].second, Bx = T[b].first, By = T[b].second;
		return (Ay * Bx - By * Ax) / (Bx - Ax);
	}
	
	// Return n = hi or lo such |Y(n)-y| is smallest
	// update hi or lo to candidate ordinate for next iteration
	int Pick(float y, int& hi, int& lo)
	{
		float U = hi < 50 ? std::abs(Y(hi) - y) : INF;
		float L = lo >= 0 ? std::abs(Y(lo) - y) : INF;
		return (L <= U) ? lo-- : hi++;
	}

	// Aitken's Iterative interpolation until reaching error bound < eps (maximum degree 8)
	float Aitken(float y, float eps = 1e-5)
	{
		int hi = std::upper_bound(_Y.begin(), _Y.end(), y) - _Y.begin();
		int lo = hi - 1;
		// estimated error in interpolation
		float err_est = INF;
		Tab T;
		for (int b = 0; b < 8; ++b)
		{
			Assert(T.size() == b);
			int n = Pick(y, hi, lo);
			T.emplace_back(Y(n) - y, X(n));
			Assert(T.size() == b + 1);
			for (int a = 0; a < b; ++a)
				T[b].second = del(T, a, b);
			// Stop when estimated error < eps
			if (std::abs(T[b].second - err_est) < eps)
				break;
			err_est = T[b].second;
		}
		return T.back().second/100.0;
	}

	// Single Checker Model
	Scm() { initDividedDifference(); }
};

Scm scmDist;

float ScmStat(int X, int Y)
{
	// 1/2mu ~ 4.0753
	float D = Y - X + 4.0753;
	float S = X + Y - 24.72588;
	// sign(D)*D^2/S
	return std::abs(D) * D / S;
}

float ScmPwin(float stat)
{
	return stat < 0 ? 1.0 - scmDist.NQI(-stat) : scmDist.NQI(stat);
}

/// <summary>
/// Single Checker Model win probability
/// with Black to Move
/// </summary>
/// <param name="W">pip count of white checkers</param>
/// <param name="B">pip count of black checkers</param>
/// <returns></returns>
float ScmPwin(int W, int B)
{
	return ScmPwin(ScmStat(W, B));
}

