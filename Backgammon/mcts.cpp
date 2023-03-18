#include "gametree.h"


int State::N()
{
	int n = 0;
	for (auto c : choices)
	{
		Node a(c);
		n += a.N();
	}
	return n;
}

Choice State::UCB1(int n)
{
	float N_r = c_puct * std::sqrt(float(n));
	float score = 0.0;
	int argmax = 0, i = 0;
	for (auto c : choices)
	{
		Node a(c);
		float upper_bound = a.Q() + N_r / a.N();
		argmax = upper_bound > score ? i : argmax;
		score = upper_bound > score ? upper_bound : score;
		++i;
	}
	return choices[argmax];
}

/// <summary>
/// Playout from this state
/// </summary>
/// <param name="n"></param>
/// <returns></returns>
ValueQ State::playout(GameTree& t, int n)
{
	return Node(UCB1(n)).playout(t);
}

Node Node::selection()
{
	Assert(!leaf());
	return Node(State(transitions()[roll_dice().ordinal]).UCB1(N()));
}
/// <summary>
/// Expected win probability
/// mean of current best Q of each state weighted by state probability
/// </summary>
/// <returns></returns>
ValueQ Node::expectedValue() 
{ 
	ValueQ q = 0.0; 
	for (auto& r : Roll::rolls21)
	{
		q += r.p * State(transitions()[r.ordinal]).value();
	}
	return q;
}

ValueQ Node::expand(GameTree& t)
{ 
	// Initialize transition ptr
	Assert(T() == nullptr);
	T() = t.push_transition();

	// Fill choice array for each State transition
	BoardInfo b(board());
	for (auto& r : Roll::rolls21)
	{
		genMoves(t, b, r);
		t.end_state();
	}
	// Update N and Q from the values ot the expanded nodes
	N() = 36;	// Should test whether this value is optimal
	return Q() = expectedValue(); 
}

void Node::update(ValueQ q) 
{
	int old_n = N()++;
	ValueQ old_q = Q();
	Q() = (q + old_n * old_q) / (1 + old_n);
}

ValueQ Node::playout(GameTree& t)
{
	if (leaf())
		return expand(t);
	// rollout
	ValueQ q = selection().playout(t);
	// backpropagate
	update(q);
	return q;
}

