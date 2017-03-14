#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
#include <ctime>

using namespace std;

class Player {

private:
	Board playBoard;
	Side mySide;
	Side otherSide;   // keep this for efficiency

public:
    Player(Side side);
    ~Player();

    void setBoard(char data[]) { playBoard.setBoard(data); }
    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
