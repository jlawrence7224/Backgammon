#include "gametree.h"

typedef float ValueQ;
typedef int ChoiceNode;


ValueQ expand(Action a) { return 0.0; }

void update(Action a, ValueQ q) {}

Action selection(Action a)
{
	return a.selection();
}
ValueQ mcts_playout(Action a)
{
	ValueQ q = a.leaf() ? expand(a) : mcts_playout(selection(a));
	// backpropagate
	update(a, q);
	return q;
}