#pragma once
#include <iostream>
#include <cstdint>
using namespace std;

class Instruction
{
private:
    uint16_t rawInstruction;

public:
    Instruction(uint16_t instruction) : rawInstruction(instruction) {}

    // Bitwise Right Shift (>>):
    /**
     * 0x1234 >> 4  => 0x0123 (drops the last hex digit '4')
     * 0x1234 >> 8  => 0x0012 (drops '34')
     *  0x1234 >> 12 => 0x0001 (drops '234')
     */

    // Bitwise AND (&) Masking:
    /**
     * 0x1234 & 0x0F => 0x04 (keeps only the last hex digit)
     * 0x1234 & 0xFF => 0x34 (keeps the last two hex digits / memory address)
     */

    uint8_t GetOpCode() const
    {
        return (rawInstruction >> 12) & 0x0F;
    }

    uint8_t GetOperand1() const
    {
        return (rawInstruction >> 8) & 0x0F;
    }

    uint8_t GetOperand2() const
    {
        return (rawInstruction >> 4) & 0x0F;
    }

    uint8_t GetOperand3() const
    {
        return rawInstruction & 0x0F;
    }

    uint8_t GetMemoryAddress() const
    {
        return rawInstruction & 0xFF;
    }
};
