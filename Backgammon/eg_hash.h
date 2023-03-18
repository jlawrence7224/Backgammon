#pragma once
#include <vector>
#include <array>
#include <cmath>
#include "inttyp.h"

struct Board;
struct BoardInfo;

constexpr int n_inner_table_configurations = 54264;	// multichoose(7,15)

// Combinatorial functions used
// in computation of inner table hash
int64 binom(int64 n, int64 k);
int64 multichoose(int64 n, int64 k);

// singleton class
struct eg_hash 
{
	using Hash			= int64;
	using BwHash		= std::pair<Hash, Hash>;
	using inner_table	= std::array<int, 7>;

	static const int n_pre = 7;
	static const int k_pre = 15;

	std::array<std::array<int64,k_pre + 2>, n_pre> multi_choose;

	eg_hash()
	{
		for (int n = 0; n < DIM(multi_choose); ++n)
			for (int k = 0; k < DIM(multi_choose[0]); ++k)
				multi_choose[n][k] = multichoose(n + 1, k - 1);
	}

	/// <summary>
	/// Precalculated multichoose coefficients
	/// used by inner table hash function
	/// </summary>
	/// <param name="n">1..7</param>
	/// <param name="k">-1..15</param>
	/// <returns></returns>
	int64 mc(int64 n, int64 k) 
	{ 
		Assert(1 <= n && n <= n_pre && -1 <= k && k <= k_pre);
		//Assert(multichoose(n, k) == multi_choose[n - 1][k + 1]);
		return multi_choose[n - 1][k + 1];
	}

	Hash S(int64 i, int64 K) { return mc(7 - i, 15 - (K + 1)); }

	/// <summary>
	/// Compute perfect hash of the inner table b
	/// x < y ==> hash(y) < hash(x)
	/// where x < y is lexicographic order
	/// </summary>
	/// <param name="b"></param>
	/// <returns></returns>
	Hash			hash		(const inner_table& b);

	/// <summary>
	/// Invert the hash function: Hash->inner_table
	/// Writes into argument b the inner table configuration with Hash h
	/// Returns b
	/// </summary>
	/// <param name="h">input hash code</param>
	/// <param name="b">output inner_table</param>
	/// <returns>b</returns>
	inner_table&	inverse_hash(Hash h, inner_table& b);

	/// <summary>
	/// Compute hash of the inner table of white pieces
	/// </summary>
	/// <param name="b"></param>
	/// <returns></returns>
	Hash			hash_w		(const Board& b);
	BwHash			hash_bw		(const Board& b);
};

extern struct eg_hash eg;	// singleton instance

/// <summary>
/// Iterator over inner table checker configurations in eg_hash hash order
/// </summary>
struct inner_table_iterator
{
	using it = inner_table_iterator;
	using inner_table = eg_hash::inner_table;
	using Hash = eg_hash::Hash;
	static const int k = 15;
	static const Hash max_index = n_inner_table_configurations - 1;;

	Hash _hash;
	std::vector<int> stack;
	inner_table checkers;

	bool more() { return _hash < max_index; }

	void set_stack() {
		stack.clear(); stack.reserve(8);
		for (int i = 0; i < 7; ++i)
			if (checkers[i] > 0)
				stack.push_back(i);
	}
	/// <summary>
	/// Initialize to 0th position -- all checkers finished
	/// </summary>
	inner_table_iterator() : _hash(0), checkers{ k, 0, 0, 0, 0, 0, 0 } { stack.reserve(8); stack.push_back(0); }

	/// <summary>
	/// Initialize to position with hash = n
	/// </summary>
	/// <param name="n"></param>
	inner_table_iterator(Hash n) : _hash(n)
	{
		eg.inverse_hash(n, checkers);
		set_stack();
	}

	Hash seq_index() { return _hash; }
	const inner_table& operator*() { Assert(_hash < n_inner_table_configurations); return checkers; }
	it& operator++()
	{
		if (++_hash >= n_inner_table_configurations)
			return *this;

		int& last = stack.back();
		if (last < 6)
		{
			checkers[last + 1] = 1;
			if (--checkers[last] > 0)
				stack.push_back(last + 1);
			else
				++last;
		}
		else
		{
			int c = checkers[6];
			checkers[6] = 0;
			stack.pop_back();
			Assert(!stack.empty());
			int& prev = stack.back();
			Assert(prev >= 0 && prev < 6);
			checkers[prev + 1] = c + 1;
			if (--checkers[prev] > 0)
				stack.push_back(prev + 1);
			else
				++prev;

		}
		Assert(_hash == eg.hash(checkers));
		return *this;
	}
};