#pragma once
#include <vector>
#include <array>
#include <cmath>
#include "inttyp.h"

namespace endgame 
{
	constexpr int n_inner_table_configs = 54264;
	constexpr int n_7_6 = 924;		// multichoose(7,6) == # of 6 checker inner board positions
	constexpr int n_7_7 = 1716;		// multichoose(7,7) == # of 7 checker inner board positions
	constexpr int n_exact = n_7_6;

	using inner_array = std::vector<float>;
	using inner_board = std::array<int, 7>;
	using Hash = int64;
	using Pexact = std::array < std::array<float, n_exact>, n_exact>;

	extern inner_array enr;
	extern Pexact p_exact;

	struct bg_hash {
		bg_hash(Hash w, Hash b) : w_hash(w), b_hash(b) {}
		Hash w_hash;
		Hash b_hash;
	};

	extern void				compute_p_exact();
	extern void				compute_enr();
	extern Hash				position_hash(const inner_board& b);
	extern inner_board&		inverse_hash(Hash h, inner_board& output);
	extern Hash				S(int i, int K);

	extern void				init_tables();
	extern void				compute_enr();
	extern float			Pwin(Hash hw, Hash hb);
	/// <summary>
	/// Iterator over inner table checker configurations in eg_hash hash order
	/// </summary>
	struct inner_table_iterator
	{
		using it = inner_table_iterator;
		static const int k = 15;
		static const Hash max_index = n_inner_table_configs - 1;;

		Hash _hash;
		std::vector<int> stack;
		inner_board checkers;

		bool more() { return _hash < max_index; }

		void set_stack() { 
			stack.clear(); stack.reserve(8);
			for (int i = 0; i < 7; ++i) 
				if (checkers[i] > 0) 
					stack.push_back(i); 
		}
		// Initialize to 0th position -- all checkers finished
		inner_table_iterator() : _hash(0), checkers{ k, 0, 0, 0, 0, 0, 0 } { stack.reserve(8); stack.push_back(0); }
		// Initialize to position with hash = n
		inner_table_iterator(Hash n) : _hash(n)
		{
			inverse_hash(n, checkers);
			set_stack();
		}

		Hash seq_index() { return _hash; }
		const inner_board& operator*() { Assert(_hash < n_inner_table_configs); return checkers; }
		it& operator++()
		{
			if (++_hash >= n_inner_table_configs)
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
			Assert(_hash == position_hash(checkers));
			return *this;
		}
	};

	struct shifted_vector
	{
		shifted_vector(size_t upper, size_t lower) : support(upper - lower), offset(lower) {}
		shifted_vector(float f) : support(1), offset(0) { support[0] = f; }
		std::vector<float> support;
		size_t  offset;

		size_t  upper() { return support.size() + offset; }
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


	using Dist = shifted_vector;
	using XDist = std::vector<Dist>;
	extern XDist Xdist;
};