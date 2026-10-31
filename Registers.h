#pragma once
#include <iostream>
#include "CellsBasic.h"
using namespace std;

class Registers : public CellsBasic
{
private:
    static constexpr int NUM_REGISTERS = 16;

public:
    Registers() : CellsBasic(NUM_REGISTERS) {}
};
