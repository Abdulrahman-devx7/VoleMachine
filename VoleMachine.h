#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Screen.h"
#include <vector>
#include <cstdint>
using namespace std;
class VoleMachine
{
private:
Screen _outputScreen;
CPU _CPU;
Memory _memory;
void LoadProgram(vector<uint16_t> instructions);
void Step();
void Reset();
public:
public:
VoleMachine();
void Start();
};