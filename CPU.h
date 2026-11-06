#pragma once
#include "ControlUnit.h"
#include "Screen.h"
#include "Registers.h"
#include "Memory.h"
using namespace std;
class CPU { 
private :
    ControlUnit controlUnit;
    Registers registers ;
    Screen& screen;
    bool isHalted=true;
public:
    CPU(Screen& screenRef )
        : controlUnit(screenRef), screen( screenRef)
    {
    }
    void Start(){
        isHalted=  false;
    }
    void Run(Memory& memory ) {
        isHalted =false;
        while (!isHalted){
            Step(memory);
        }
    }
    void Step( Memory& memory ){
        if (isHalted )
            return;
        controlUnit.Fetch(memory);
        controlUnit.Execute(memory, registers, isHalted);
    }
    void Reset(){
        controlUnit.Reset();
        registers.Reset();
        screen.Clear() ;
        isHalted=true ;
    }
    bool IsHalted() const
    {
        return isHalted;
    }
    void SetPC(uint8_t address ){
        controlUnit.SetPC( address);
    }
};
