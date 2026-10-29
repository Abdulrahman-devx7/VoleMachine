#pragma once
#include "Screen.h"
#include <cstdint>

using namespace std;
class ControlUnit
{
private:
	Screen &_outputScreen;
	uint16_t _IR = 0x0000;
	uint8_t _PC = 0x00;

public:

	ControlUnit(Screen &voleScreen)
		: _outputScreen(voleScreen)
	{
	}
		
};

