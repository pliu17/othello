#include "board.hpp"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}


/*
 * Helper function: to find all the legal moves for the specified side using checkMove()
 */
vector<int> Board::getLegalMoveIds(Side side) {
    vector<int> legalMoveIds;
    for (int x = 0; x < 8; ++x)
    {
        for (int y = 0; y < 8; ++y)
        {
            Move move(x, y);
            if (checkMove(&move, side))
                legalMoveIds.push_back(x + 8*y);  // trace the moveId for a legal move
        }
    }
    return legalMoveIds;
}

/*
 * Helper function: to find the best legal move with from all legal moves
 * This function can uses a simple scheme, score = (# stones you have) - (# stones your opponent has)
 * or it can use a heuristic score with a multiplier of 3 for corners, and -3 for squares adjacent to corners. 
 */

int Board::getBestMoveId(Side side, vector<int>& legalMoveIdVec)
{
    if (legalMoveIdVec.size() < 1)
        return -1;

    int bestScore = INT_MIN;
    int bestId = -1;
    for (int moveId: legalMoveIdVec)
    {
        // copy the current board to a simulated board for a next move
        Board *nextBoard = copy();

        int x = moveId%8;
        int y = moveId/8;
        Move nextMove(x, y);
        nextBoard->doMove(&nextMove, side);
        //int score = nextBoard->calcSimpleScore(side);
        int score = nextBoard->calcHeuristicScore(side, nextMove);
        //if (score != score0)
        //    cerr << "Something fishy: (score0, score)=(" << score0 << "," << score << ")" << endl;
        if (score > bestScore)
        {
            bestScore = score;
            bestId = moveId;
        }

        delete nextBoard;  // clean up the simulated board
    }

    return bestId;

}


/*
 * Helper function: to get the current score using a simple scheme:
 *                  score = (# stones the given side has) - (# stones the opponent side has)
 */
int Board::calcSimpleScore(Side side)
{
    return side==BLACK ? countBlack()-countWhite() : countWhite()-countBlack();
}


/*
 * Helper function: to get the current score using a heuristic scheme (position-weighted):
 *                  score = (# stones the given side has) - (# stones the opponent side has)
 */
int Board::calcHeuristicScore(Side side, Move &testMove)
{
    int x = testMove.x;
    int y = testMove.y;
    int multiplier = 1;
    if ( (x==0 || x==7) && (y==0 || y==7) )  // corners
        multiplier = 3;
    else if ( (x<=1 || x >=6) && (y<=1 || y >= 6))  // corner-adjacent squares + corners
        multiplier = -3;

    return side==BLACK ? multiplier*countBlack()-countWhite() : multiplier*countWhite()-countBlack();
    //return multiplier*calcSimpleScore(side);
}



/*
 * Helper function: to find the best legal move with from all legal moves
 *  1. Calculate all legal moves to be considered
 *  2. Based on a score scheme to calculate a score for each legal move, find the move id with the best (max) score.
 *  3. return the move with the best score as the next move
 *
 * This function will return nullptr if no legal move can be found
 */
Move *Board::getBestNextMove(Side side)
{
    vector<int> legalMoveIdVec = getLegalMoveIds(side);
    if (legalMoveIdVec.size() < 1)  // no legal move to explore
        return nullptr;

    // string legalMovesString("[");
    // string scoresString("]");
    // ostringstream legalMovesSS;
    // ostringstream scoresSS;
    // legalMovesSS << "[";
    // scoresSS << "[";
    int bestScore = INT_MIN;
    int bestId = -1;
    for (int moveId: legalMoveIdVec)
    {
        // copy the current board to a simulated board for a test move
        Board *testBoard = copy();

        Move testMove(moveId%8, moveId/8);
        testBoard->doMove(&testMove, side);
        //int score = testBoard->calcSimpleScore(side);
        int score = testBoard->calcHeuristicScore(side, testMove);
        // legalMovesSS << "(" << testMove.x << "," << testMove.y << "),";
        // scoresSS << score << "," ;

        if (score > bestScore)
        {
            bestScore = score;
            bestId = moveId;
        }

        delete testBoard;  // clean up the simulated board
    }
    // legalMovesString += "]";
    // scoresString += "]";
    // legalMovesSS << "]";
    // scoresSS << "]";

    // cerr << "getBestNextMove(): legalMoves=" + legalMovesSS.str() << endl;
    // cerr << "getBestNextMove(): scores=" + scoresSS.str() << endl;

    if (bestId < 0)
    {
        cerr << "getBestNextMove(): fishy score calculation: no score exists for all legal moves of size="
            << legalMoveIdVec.size() << endl;
        return nullptr;
    }

    Move *nextMove = new Move(bestId%8, bestId/8);

    return nextMove;
}


/*
 * Helper function: to find the best legal move with from all legal moves
 *  1. Calculate all legal moves to be considered
 *  2. Based on the minimax decision tree to find the move id with the best (max) score.
 *  3. return the move with the best score as the next move
 *
 * This function will return nullptr if no legal move can be found
 */
Move *Board::getMiniMaxMove(Side side, int lookAheadLevel)
{
    vector<int> legalMoveIdVec = getLegalMoveIds(side);
    if (legalMoveIdVec.size() < 1)  // no legal move to explore
        return nullptr;

    // string legalMovesString("[");
    // string scoresString("]");
    // ostringstream legalMovesSS;
    // ostringstream scoresSS;
    // legalMovesSS << "[";
    // scoresSS << "[";
    int bestScore = INT_MIN;
    int bestId = -1;
    for (int moveId: legalMoveIdVec)
    {
        // copy the current board to a simulated board for a test move
        Board *testBoard = copy();

        Move testMove(moveId%8, moveId/8);
        testBoard->doMove(&testMove, side);
        //int score = testBoard->calcSimpleScore(side);
        int score;
        score = testBoard->calcMiniScore(side, side==BLACK? WHITE: BLACK, testMove, lookAheadLevel, 1);


        // legalMovesSS << "(" << testMove.x << "," << testMove.y << "),";
        // scoresSS << score << "," ;

        if (score > bestScore)
        {
            bestScore = score;
            bestId = moveId;
        }

        delete testBoard;  // clean up the simulated board
    }
    // legalMovesString += "]";
    // scoresString += "]";
    // legalMovesSS << "]";
    // scoresSS << "]";

    // cerr << "getMiniMaxMove(): legalMoves=" + legalMovesSS.str() << endl;
    // cerr << "getMiniMaxMove(): scores=" + scoresSS.str() << endl;

    if (bestId < 0)
    {
        cerr << "getBestNextMove(): fishy score calculation: no score exists for all legal moves of size="
            << legalMoveIdVec.size() << endl;
        return nullptr;
    }

    Move *nextMove = new Move(bestId%8, bestId/8);

    return nextMove;
}

int Board::calcMiniScore(Side mySide, Side testSide, Move &testMove, int lookAheadLevel, int currLevel)
{
    // Base case 1: when lookAheadLevel is reached, just return the heuristic score
    if (currLevel >= lookAheadLevel)
        return calcSimpleScore(mySide); // calcHeuristicScore(mySide, testMove);   // score is calculated for mySide

    // Base case 2: or when no more legal move available; just return INT_MAX as a token for a terminal path
    vector<int> legalMoveIdVec = getLegalMoveIds(testSide);
    if (legalMoveIdVec.size() < 1)  // no legal move to explore
        return INT_MAX;  // use the INT_MAX to mean a disconnected path


    int worstScore = INT_MAX;
    // int worstId = -1;

    for (int moveId: legalMoveIdVec)
    {
        // copy the current board to a simulated board for a test move
        Board *testBoard = copy();

        Move testMove(moveId%8, moveId/8);
        testBoard->doMove(&testMove, testSide);   // move is determined by testSide
        int score = testBoard->calcMiniScore(mySide, testSide==BLACK?WHITE:BLACK, testMove, lookAheadLevel, currLevel+1);   // score is calculated for mySide
        if (score < worstScore)
        {
            worstScore = score;
            // worstId = moveId;
        }
        delete testBoard;  // clean up the simulated board
    }

    return worstScore;
}
