#include "roll.h"
#include <random>

typedef std::mt19937_64 PRNG;
PRNG rng(uint64_t(123456));
std::uniform_int_distribution<int> dist(0,35);

std::array<const Roll, 36> rolls36 = {
	Roll(1,1), Roll(1,2), Roll(1,3), Roll(1,4), Roll(1,5), Roll(1,6),
	Roll(2,1), Roll(2,2), Roll(2,3), Roll(2,4), Roll(2,5), Roll(2,6),
	Roll(3,1), Roll(3,2), Roll(3,3), Roll(3,4), Roll(3,5), Roll(3,6),

	Roll(4,1), Roll(4,2), Roll(4,3), Roll(4,4), Roll(4,5), Roll(4,6),
	Roll(5,1), Roll(5,2), Roll(5,3), Roll(5,4), Roll(5,5), Roll(5,6),
	Roll(6,1), Roll(6,2), Roll(6,3), Roll(6,4), Roll(6,5), Roll(6,6),
};

const Roll& roll_dice()
{
	return rolls36[dist(rng)];
}

// 6.6	24	20
// 5.5  20	19
// 4.4	16	18
// 3.3	12	17
// 6.5	11	16
// 6.4  10	15
// 5.4	 9	14
// 6.3	 9	13
// 2.2	 8	12
// 5.3	 8	11
// 6.2	 8	10
// 4.3	 7	 9
// 5.2	 7	 8
// 6.1	 7	 7
// 4.2	 6	 6
// 5.1	 6	 5
// 3.2	 5	 4
// 4.1	 5	 3
// 1.1	 4	 2
// 3.1	 4	 1
// 2.1	 3	 0
const int Roll::order[6][6] = { // lo.hi
	{  2,  0,  1,  3,  5,  7},
	{  0, 12,  4,  6,  8, 10},
	{  0,  0, 17,  9, 11, 13},
	{  0,  0,  0, 18, 14, 15},
	{  0,  0,  0,  0, 19, 16},
	{  0,  0,  0,  0,  0, 20}
};

const std::vector<Roll> Roll::rolls21 = [] {
	std::vector<Roll> r;
	for (int i = 0; i < 6; ++i)
		for (int j = i; j < 6; ++j)
			r.push_back(rolls36[i * 6 + j]);
	// sort the rolls with high ordinal first (pip_count_1,lo_1) > (pip_count_2,lo_2)
	// 6.6 > 5.5 > ... > 2.1
	std::sort(r.begin(),r.end(),std::greater<Roll>());
	return r;
}();