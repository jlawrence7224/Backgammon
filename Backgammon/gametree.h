#pragma once

#include "inttyp.h"
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include "roll.h"
#include "range.h"
#include "board.h"

#if 0
enum Color {
	White = 0,
	Black = 1,
};
Color operator~(Color c) { return Color(c ^ 1); }
#endif


template<int N>
struct Transitions;

struct BoardVal
{
	BoardVal() : n(0), Q(0.0), t((Transitions<1>*)0) {}

	int			    n;			// Count of # times this action taken
	float		    Q;			// Estimated value
	Transitions<1>* t;
};

using Tree = std::map<Board, BoardVal>;
using Choice = Tree::iterator;
using State = Rng<Choice>;

template<int N>
struct Transitions {
	std::array<int, 22> offset;
	Choice base[N];

	State operator[] (int i)
	{
		Choice* t_start = &base[offset[i]];
		Choice* t_end = &base[offset[i + 1]];
		return State(t_start, t_end);
	}
};

const float c_puct = 1.414f;		// sqrt(2)

struct Action {
	using value_type = Tree::value_type;

private:
	value_type& node;
public:
	Action(Choice c) : node(*c) {}

	const
	Board&			board()			{ return node.first; }
	int&			N()				{ return node.second.n; }
	float&			Q()				{ return node.second.Q; }
	bool			leaf()			{ return 0 == node.second.t; }
	Transitions<1>& transitions()	{ return *node.second.t; }

	/// <summary>
	/// apply this Action -- yielding a specific state
	/// The transition function is random, resulting in one of 21 possible states.
	/// </summary>
	/// <param name="s"></param>
	/// <returns></returns>
	State	apply() 
	{
		Assert(!leaf());
		return transitions()[roll_dice().ordinal]; 
	}

	Action	ucb(State s)
	{
		float N_r = c_puct*std::sqrt(float(N()));
		float score = 0.0;
		int argmax = 0, i = 0;
		for (auto c : s)
		{
			Action a(c);
			float upper_bound = a.Q() + N_r / a.N();
			argmax = upper_bound > score ? i : argmax;
			score = upper_bound > score ? upper_bound : score;
			++i;
		}
		return Action(s[argmax]);
	}

	Action	selection()
	{
		Assert(!leaf());
		return ucb(apply());
	}

};

class GameTree
{
	using Tree = std::map<Board, BoardVal>;
	using Choice = Tree::iterator;
	using State = Rng<Choice>;

	using Container = std::vector<Choice>;
	using size_t = Container::size_type;
	using tptr = Transitions<0>*;
	static const size_t default_size = 1024 * 1024;
	static const size_t tsize = sizeof(Transitions<0>) / sizeof(Choice);

	Tree		tt;
	Container	data;

	tptr		t;
	int			t_beg;
	int			s_cnt;

public:
	GameTree(Container::size_type sz = default_size) : data(), t(0), t_beg(0), s_cnt(21)
	{
		data.reserve(sz);
		Assert(sizeof Transitions<1> == sizeof(Transitions<0>) + sizeof(Choice));
		Assert(sizeof Transitions<0> == tsize * sizeof Choice);
	}

	void end_state() { Assert(s_cnt < 21); t->offset[++s_cnt] = data.size() - t_beg; }
	tptr push_transition()
	{
		Assert(s_cnt == 21);
		data.resize(data.size() + tsize);

		t = reinterpret_cast<tptr>(&data[data.size() - tsize]);
		t_beg = data.size();
		s_cnt = 0;
		t->offset[0] = 0;
		return t;
	}
	float eval(const Board& b) { return 0.5; }
	void push_choice(Choice c) { data.emplace_back(c); }
	void push_board(Board& b)
	{
		auto cb = tt.try_emplace(b);
		Choice c = cb.first;
		if (!cb.second)
			c->second.Q = eval(b);
		push_choice(c);
	}
	void genMoves(Board& b, Info& info, Roll& dice)
	{
		return b.genMoves(*this, info, dice.hi, dice.lo);
	}

};












