#pragma once
#include "ControlUnit.h"
#include "Screen.h"
#include "Registers.h"
#include "Memory.h"

using namespace std;

class CPU
{
private:
    ControlUnit controlUnit ;
    Registers registers;
    Screen& screen;
    bool isHalted=true ;
    bool hasError  =false;

public:
    CPU(Screen& screenRef )
        : controlUnit(screenRef),screen(screenRef)
    {
    }

    void Start()
    {
        hasError =false;
        isHalted=false;
    }

    void Run(Memory& memory )
    {
        Start();

        while (!isHalted && !hasError)
            Step(memory);
    }

    void Step( Memory& memory)
    {
        if (isHalted || hasError)
            return;

        controlUnit.Fetch( memory );

        if (!controlUnit.Execute(memory, registers, isHalted))
            hasError = true;
    }

    void Reset() {
    
        controlUnit.Reset();
        registers.Reset();
        screen.Clear();
        isHalted = true;
        hasError = false;
    }

    bool IsHalted() const{
    
        return isHalted;
    }

    bool HasError() const
    {
        return hasError;
    }

    void SetPC(uint8_t address)  {
    
        controlUnit.SetPC(address);
    }
};

