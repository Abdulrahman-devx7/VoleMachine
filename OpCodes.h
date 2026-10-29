#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

// The 12 op-code operations for the Brookshear Vole machine.
enum class OpCode : short {
    LoadMemory = 1,  // 1RXY: LOAD Reg R with contents of Memory[XY]
    LoadImmediate = 2,  // 2RXY: LOAD Reg R with literal value XY
    StoreMemory = 3,  // 3RXY: STORE Reg R to Memory[XY], or to the screen at FF
    MoveRegister = 4,  // 40RS: COPY contents of Reg R into Reg S
    AddTwos = 5,  // 5RST: ADD Reg S and Reg T as two's-complement values -> Reg R
    AddFloat = 6,  // 6RST: ADD Reg S and Reg T as floating-point values -> Reg R
    BitwiseOr = 7,  // 7RST: BITWISE OR Reg S and Reg T -> Reg R
    BitwiseAnd = 8,  // 8RST: BITWISE AND Reg S and Reg T -> Reg R
    BitwiseXor = 9,  // 9RST: BITWISE XOR Reg S and Reg T -> Reg R
    Rotate = 10, // AR0X: ROTATE Reg R right by X bits
    JumpEq = 11, // BRXY: JUMP to XY if Reg R == Reg 0
    Halt = 12  // C000: HALT execution
};

#endif // OPCODES_H