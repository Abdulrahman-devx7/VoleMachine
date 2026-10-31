#pragma once
#include <iostream>
#include "CellsBasic.h"
using namespace std;

class Memory : public CellsBasic
{
private:
    static constexpr int NUM_CELLS = 256;
public:
    Memory() : CellsBasic(NUM_CELLS) {}
};
