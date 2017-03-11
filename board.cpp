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
        int score = nextBoard->calcSimpleScore(side);
        //int score = nextBoard->calcHeuristicScore(side);
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
int Board::calcHeuristicScore(Side side)
{
    return side==BLACK ? heuristicScore(BLACK)-heuristicScore(WHITE) : heuristicScore(WHITE)-heuristicScore(BLACK);
}


/*
 * Helper function: a heuristic scheme (position-weighted):
 *                  score = (# position scores the given side has) - (# position scores the opponent side has)
 *                  corner scores are emphasized by a multiplier of 3
 *                  corner-adjacent scores are de-emphasized by a multiplier of 3
 */

int Board::heuristicScore(Side side)
{
    int multiplier = 1;
    int total = count(side);

    // // emphasize corners
    // vector<Move> cornerVec = { Move(0, 0), Move(7, 0), Move(0, 7), Move(7, 7) };
    // for (Move &c: cornerVec)
    // {
    //     int X = c.getX();
    //     int Y = c.getY();
    //     for (int dx = -1; dx <= 1; ++dx)
    //     {
    //         for (int dy = -1; dy <= 1; ++dy)
    //         {
    //             int x = X + dx;
    //             int y = Y + dy;
    //             if (x < 0 || x > 7 || y < 0 || y > y)
    //                 continue;
    //             if (dx == 0 && dy == 0)  // corners
    //                 total += extraWeight * get(side, x, y);  //  emphasize corners
    //             else
    //                 total -= extraWeight * get(side, x, y);  // de-emphasize squares adjacent to corners
    //         }
    //     }
    // }

    // emphasize corners
    // (0, 0), (7, 0), (0, 7), (7, 7)
    total += (multiplier-1) * get(side, 0, 0);
    total += (multiplier-1) * get(side, 7, 0);
    total += (multiplier-1) * get(side, 0, 7);
    total += (multiplier-1) * get(side, 7, 7);

    // de-emphasize squares adjacent to corners
    // (1,0), (0, 1), (1, 1); (6, 0), (7, 1), (6, 1); (1, 7), (0, 6), (1, 6); (6, 7), (7, 6), (6, 6)
    total -= (multiplier+1) * get(side, 1, 0);
    total -= (multiplier+1) * get(side, 0, 1);
    total -= (multiplier+1) * get(side, 1, 1);

    total -= (multiplier+1) * get(side, 6, 0);
    total -= (multiplier+1) * get(side, 7, 1);
    total -= (multiplier+1) * get(side, 6, 1);

    total -= (multiplier+1) * get(side, 1, 7);
    total -= (multiplier+1) * get(side, 0, 6);
    total -= (multiplier+1) * get(side, 1, 6);

    total -= (multiplier+1) * get(side, 6, 7);
    total -= (multiplier+1) * get(side, 7, 6);
    total -= (multiplier+1) * get(side, 6, 6);

    return total;
}
