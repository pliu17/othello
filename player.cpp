#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */

    // playBoard is an instance, it is created when Player constructor starts
    clock_t beginTime = clock();  // snapshot the clock for starting
    mySide = side;
    otherSide = (mySide == BLACK) ? WHITE : BLACK;
    cerr << "Side = " << (side==BLACK? "BLACK" : "WHITE") << endl;

    double elapsed_msec = double(clock() - beginTime)/CLOCKS_PER_SEC * 1000;

    if (elapsed_msec > 30000)
        cerr << "Why is the contructor of Player taking longer than 30 seconds.." << endl;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    /*
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */
    clock_t beginTime= clock(); // snapshot the clock for starting

    // first we check whether the opponentsMove is not nullptr and then we need to check if it is legal
    if (opponentsMove != nullptr && !playBoard.checkMove(opponentsMove, otherSide))
        cerr << "Side " << (otherSide==WHITE ? "WHITE": "BLACK") << " are making an illegal move" << endl;

    // Modifies the board to reflect the specified move.
    playBoard.doMove(opponentsMove, otherSide);


    // now we check every legal move
    vector<int> legalMoveIdVec = playBoard.getLegalMoveIds(mySide);
    if (legalMoveIdVec.size() == 0) // no legal move
        return nullptr;
    // for (int id : legalMoveIdVec)
    // {
    //     cerr << "(x,y)=(" << id%8 << "," << id/8 << ")" << endl;
    // }

    // now we check some legal move
    // int id = legalMoveIdVec[0];
    int id = playBoard.getBestMoveId(mySide, legalMoveIdVec);


    Move *myMove = new Move(id%8, id/8);

    double elapsed_msec = double(clock() - beginTime)/CLOCKS_PER_SEC * 1000;
    if (msLeft > -1 && msLeft < elapsed_msec)
        cerr << "No time left" << endl;

    // Before return myMove, update playBoard
    playBoard.doMove(myMove, mySide);

    return myMove;
}
