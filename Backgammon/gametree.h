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

// Design notes:
//
// MCTS game tree can be abstractly described by the following relationships
//
// State:	A State determines a set of Actions which can be chosen when in that State.
// Node:	A possible 'move', 'behavior', or 'act' available in a State
// Transition: Applying an 'action' to a 'state' puts the game into another state.
//			   In probabilistic games only the probability of ending in a particular
//			   state is known.
//
// a :	State -> {Node}
//		A mapping from a State to the set of Actions (moves) available in a State
//
//	 *	equivalently describe as the relation
//
// A :	(s:State,a:Node) -> Bool
//		A(s,a) is true iff a is available in state s (a is member of a(s))
//
// p :	(s_curr:State,a:Node,s_next:State) -> probability
//		p(s_curr,a,s_next) = the probability that taking Node a 
//		in State s_curr results in game state s_next
//
//	 *	equivalently define a function from (State,Node) pairs to
//		random variables whose outcomes are possible States.
//
// T :	(s_curr:State,a:Node) -> S
//		We call a (State,Node) pair: a Choice
// Choice : (s:State,a:Node) where A(s,a)
// T :	Choice -> S
//
// S :	A random variable whose outcomes are States satisfying
//		P(T(s_curr,a) = s) = p(s_curr,a,s)
//
// The MCTS algorithm assigns values to (State,Node) pairs
// Val:	(State,Node) -> Value
// Val:	Choice -> Value
// such that optimal choice in state s is the action a with optimal Val
// Val(s,a) >= Val(s,a') for all a' such that A(s,a')
//

// MCTS estimates a Value for each Choice assuming that Actions selected
// for each consequent State will constitute the optimal Choice

// In Backgammon we have the following concepts
//
// Board:	A description of the location of all of the players' checkers
//			for each player: # of the bar, # finished, # on each pip
// Roll	:	The random variable representing the outcome of a dice roll
//			21 possible (backgammon distinguishable) outcomes with standard probabilities.
// Move	:	The transformation of one Board into another. Can be described as an
//			element of Move = (Board X Board) : (board_from,board_to). A move is
//			'legal' only if board_from = the board we are moving from, and board_to
//			can be obtained by advancing checkers consistent with a particular dice Roll
//
// Node:	corresponds to Move
// using Node = Move
//
// State:	A State determines the set of Actions available at that State.
//			In backgammon a Board combined with a dice roll consitutes a 'State'
//			as that is what determines what Moves/Actions are available
// using State = (Board,Roll);

// S :	A random variable whose outcomes are the possible States 
//		Since a State is a (Board,Roll) pair, an S corresponds to a Board.
//		An S is represented as Board paired with an array of States indexed by dice roll ordinals.
//		This Board/StateArray pair (called a BoardVal) is stored in the GameTree structure.
//
//
// Choice:	the result of applying an Node to the state.
//			It is represented as a (pointer to the) Board resulting by making
//			a move (Node) from the current board with the given roll (which 
//			together determine the state).
//			A State is represented as the set of Choices (the outcomes of an Node)
//			available from that state.

typedef float ValueQ;
typedef int ChoiceNode;

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
using Choices = Rng<Choice>;
//using State = Rng<Choice>;

template<int N>
struct Transitions {

	std::array<int, 22> offset;
	Choice base[N];

	Choices operator[] (int i)
	{
		Choice* t_start = &base[offset[i]];
		Choice* t_end = &base[offset[i + 1]];
		return Choices(t_start, t_end);
	}
};

const float c_puct = 1.414f;		// sqrt(2)

class GameTree
{
public:
	using Tree = std::map<Board, BoardVal>;
	using Choice = Tree::iterator;
	using State = Rng<Choice>;

	using Container = std::vector<Choice>;
	using size_t = Container::size_type;
	using Tran = Transitions<1>*;
	static const size_t default_size = 1024 * 1024;
	static const size_t tsize = sizeof(Transitions<0>) / sizeof(Choice);

private:
	Tree		tt;
	Container	data;

	// These three members are manipulated by push_board
	// in a stateful, episodic fashion to construct the
	// Transition array when expanding leaf nodes.
	Tran		T;
	int			t_beg;
	int			s_cnt;

public:
	GameTree(Container::size_type sz = default_size) : data(), T(0), t_beg(0), s_cnt(21)
	{
		data.reserve(sz);
		Assert(sizeof Transitions<1> == sizeof(Transitions<0>) + sizeof(Choice));
		Assert(sizeof Transitions<0> == tsize * sizeof Choice);
	}

	Tran push_transition()
	{
		Assert(s_cnt == 21);
		data.resize(data.size() + tsize);

		T = reinterpret_cast<Tran>(&data[data.size() - tsize]);
		t_beg = data.size();
		s_cnt = 0;
		T->offset[0] = 0;
		return T;
	}
	void end_state() { Assert(s_cnt < 21); T->offset[++s_cnt] = data.size() - t_beg; }

	void push_choice(Choice c) { data.emplace_back(c); }
	/// <summary>
	/// Find/insert board into tree and 
	/// initialize BoardVal of new boards.
	/// Push the corresponding choice (tree iterator)
	/// onto the Choice array of State being constructed.
	/// </summary>
	/// <param name="b"></param>
	void push_board(Board& b)
	{
		auto cb = tt.try_emplace(b);
		Choice c = cb.first;
		if (!cb.second)
		{
			c->second.Q = b.eval();
			c->second.n = 1;
		}
		push_choice(c);
	}
	void genMoves(Board& b, Info& info, Roll& dice)
	{
		return b.genMoves(*this, info, dice.hi, dice.lo);
	}
};



struct Node {
	using value_type = Tree::value_type;
	using tptr = GameTree::Tran;

private:
	value_type& node;
public:
	Node(Choice c) : node(*c) {}

	const Board&	board()			{ return node.first; }
	int&			N()				{ return node.second.n; }
	float&			Q()				{ return node.second.Q; }
	bool			leaf()			{ return 0 == node.second.t; }
	tptr&			T()				{ return node.second.t; }
	Transitions<1>& transitions()	{ return *T(); }

	/// <summary>
	/// Select current best action of the state resulting from a dice roll
	/// 
	/// Random realization of this S/Node 
	/// -- yielding one of 21 possible states.
	/// The best action is selected from that state.
	/// </summary>
	/// <returns>Node determined by best action of state yielded by dice roll</returns>
	Node	selection();
	ValueQ  expectedValue();
	ValueQ	expand(GameTree&);
	void	update(ValueQ q);
	ValueQ	playout(GameTree&);
};

struct State
{
	Rng<Choice> choices;
	State(Choices cs) : choices(cs) {}
	State() : choices() {}

	int N();

	Choice UCB1(int n);
	Choice UCB1() { return UCB1(N()); }

	/// <summary>
	/// Playout from this state
	/// </summary>
	/// <returns></returns>
	ValueQ playout(GameTree& t, int n);

	/// <summary>
	/// Return choice with greatest Q value
	/// </summary>
	/// <returns></returns>
	Choice choice() { return UCB1(0); }
	ValueQ value() { return Node(choice()).Q(); }
};
using Budget = int;

struct Player 
{
	using Tran = GameTree::Tran;

	GameTree&	t;
	Board&		root;				// Black to play

	Player(GameTree& tree, Board& board) : t(tree), root(board) {}

	void	new_board(Board& board)	{ root = board; }
	bool	OfferDbl()				{ return false; }
	bool	AcceptDbl()				{ return true; }

	Node	BestChoice(State& s, Budget budget)
	{
		// Run MCTS playouts from state s
		// until our computational budget is exhausted.
		// Then return the best choice.
		int n = 0;
		do {
			s.playout(t,++n);
		} while (--budget > 0);
		return Node(s.choice());
	}

	Node	BestChoice(Roll& R, Budget budget)
	{
		Tran T = t.push_transition();
		// Fill choice array for each State transition
		BoardInfo b(root);
		for (auto& r : Roll::rolls21)
		{
			if (r == R)
				genMoves(t, b, r);
			t.end_state();
		}
		State s((*T)[R.ordinal]);
		return BestChoice(s, budget);
	}
};








