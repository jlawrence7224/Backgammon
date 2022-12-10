#pragma once

#include <array>
#include <vector>

using Die = int;

struct Roll {
	static const int order[6][6];	// lo.hi
	static const std::vector<Roll> rolls21;

	Roll(int d1, int d2) {
		hi = std::max(d1, d2);
		lo = std::min(d1, d2);
		ordinal = order[lo - 1][hi - 1];
		p = float(hi == lo ? 1.0 / 36 : 2.0 / 36);
	}

	int	hi;			// 1..6
	int lo;			// 1..6
	int	ordinal;	// 0..20	-- 21 distinct rolls, doubles at the end
	double p;		// The probability of this roll

	bool operator<   (const Roll& r)	const { return ordinal < r.ordinal;		}
	bool operator>   (const Roll& r)	const { return ordinal > r.ordinal; }
	bool operator==  (const Roll& r)	const { return ordinal == r.ordinal;	}
	bool doubles()						const { return hi == lo;				}
	bool pipCount()  					const { return hi == lo ? 4*hi : hi+lo; }
};

extern const Roll& roll_dice();