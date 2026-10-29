#pragma once
#include "ControlUnit.h"
#include "Screen.h"
#include "Registers.h"
using namespace std;

class CPU
{
private:
	ControlUnit _CU;
	Registers _register;
	Screen &_outputScreen;

	bool isHalted = true;

public:

	CPU(Screen &voleScreen)
		: _outputScreen(voleScreen), _CU(voleScreen)
	{
	}

};

