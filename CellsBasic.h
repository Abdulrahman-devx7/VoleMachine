#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <string>
#include <stdexcept>
using namespace std;

class CellsBasic
{
protected:
    const int NUM_CELLS;
    vector<uint8_t> cells;

    void TestBounds(uint8_t index) const
    {
        if (index >= NUM_CELLS)
        {
            throw out_of_range("Index out of bounds, Valid range: 0 to " + to_string(NUM_CELLS - 1) + ".\n");
        }
    }
public:
    CellsBasic(int numCells) : NUM_CELLS(numCells), cells(NUM_CELLS, 0) {}

    uint8_t GetCell(uint8_t index) const
    {
        TestBounds(index);
        return cells.at(index);
    }

    void SetCell(uint8_t index, uint8_t data)
    {
        TestBounds(index);
        cells.at(index) = data;
    }

    void Reset()
    {
        fill(cells.begin(), cells.end(), 0);
    }

    int GetSize() const
    {
        return NUM_CELLS;
    }
};
