Bonus Points!

Since the group consists of just me, all the contributions the past
two weeks are from me alone.

Improvements to AI.
Instead of playing randomly, I first wrote the two heuristic functions
suggested in the assignment (1. maximizing the difference between the
stones you and your opponent have and 2. favoring corner positions and
avoiding positions that lead to corner positions).

We then further improved the AI with by implementing 2-ply minimax
decision trees. The AI now looks 2-ply deep (eg. a turn by white and
a turn by black) in order to decide which move is the best move to make.

Hopefully, the heuristics (especially the general strategy of taking corners)
alongside looking a bit further into the game (2-ply) before making a decision
will allow the AI to perfom decently on the test.

Summary of Some Test Results

AI(Random) vs. SimplePlayer :: 5-5
AI(Both Heuristics) vs SimplePlayer :: 9-1
AI(Both Heuristics) vs ConstantTimePlayer :: 7-3
AI(2-ply Minimax with 'Simple" Heuristic [ie. Difference]) vs SimplePlayer :: 9-1
AI(2-ply Minimax with 'Simple" Heuristic [ie. Difference]) vs ConstantTimePlayer :: 3-7
AI(2-ply Minimax with Both Heuristics) vs SimplePlayer :: 2-8
AI(2-ply Minimax with Both Heuristics) vs ConstantTimePlayer :: 2-8
