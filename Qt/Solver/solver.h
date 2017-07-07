#ifndef SOLVER_H
#define SOLVER_H

#define UNIT(x,y,v) solver->data[x-1][y-1][v-1] = true; solver->checked[x-1][y-1] = v;

class Solver
{
public:
    Solver();
    //data storage in boolean format
    bool data[9][9][9];

    //the fixxed data or not
    int checked[9][9];

    //number of possibilities in a cell
    int nominee[9][9];

    //the amount of unfixxed less
    int countCell;
    int countPossibility;

    //generate the hidden value in the first time
    void start(void);

    //print matrix to debug
    void currentValue(void);

    //check for sloved cell; and turn it into fixxed cell
    bool checkCell(void);

    //check possibilities by eliminate
    bool checkPossible(void);

    //
    bool hiddenSingle(void);

    //
    bool nakedPair(void);
};

#endif // SOLVER_H
