#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include <vector>
#include <climits>
#include <iostream>
#include "common.hpp"


using namespace std;

class Board {

private:
    bitset<64> black;
    bitset<64> taken;

    bool occupied(int x, int y);
    bool get(Side side, int x, int y);
    void set(Side side, int x, int y);
    bool onBoard(int x, int y);

public:
    Board();
    ~Board();
    Board *copy();

    bool isDone();
    bool hasMoves(Side side);
    bool checkMove(Move *m, Side side);
    void doMove(Move *m, Side side);
    int count(Side side);
    int countBlack();
    int countWhite();

    void setBoard(char data[]);

    // helper functions


    // a helper function to return all legal moveId for a side
    vector<int> getLegalMoveIds(Side side);

    // a helper function to return the best moveId given all the legal move for the side
    int getBestMoveId(Side side, vector<int>& legalMoveIdVec);
    // a helper function to calculate a simple position score
    int calcSimpleScore(Side side);
    // a helper function to calculate a position-weighted heuristic score
    int calcHeuristicScore(Side side);
    int heuristicScore(Side side);


};

#endif
