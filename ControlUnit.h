#pragma once
#include "Screen.h"
#include "Memory.h"
#include "Registers.h"
#include "Instruction.h"
#include "ALU.h"
#include "OpCodes.h"
#include <cstdint>
using namespace std;
class ControlUnit {
private:
    uint8_t programCounter =0;
    uint16_t instructionRegister= 0;
    Screen& screen;
public:
    ControlUnit(Screen& screenRef ) 
        : screen( screenRef ){
    }
    void Reset(){
        programCounter = 0;
        instructionRegister=0;
    }
    uint8_t GetPC() const{
        return programCounter;
    }
    void SetPC(uint8_t address ){
        programCounter = address;
    }
    uint16_t GetIR() const {
        return instructionRegister;
    }
    void SetIR( uint16_t instruction){
        instructionRegister = instruction;
    }
    void Fetch(Memory& memory)
    {
        uint8_t highByte = memory.GetCell(programCounter);
        uint8_t lowByte = memory.GetCell(
            static_cast<uint8_t>(programCounter +1));
        instructionRegister =
            (static_cast<uint16_t>(highByte) << 8) | lowByte;
        programCounter = static_cast<uint8_t>(programCounter + 2);
    }
    void Execute(Memory& memory, Registers& registers, bool& isHalted){
        Instruction instruction(instructionRegister);
        uint8_t opcode = instruction.GetOpCode();
        if (opcode == 0 || opcode == static_cast<uint8_t>(OpCode::Halt)) {
            isHalted = true;
            return;
        }
        switch (static_cast<OpCode>(opcode)){
        case OpCode::LoadMemory:
            ExecuteLoad(instruction, memory, registers);
            break;
        case OpCode::LoadImmediate:
            registers.SetCell(
                instruction.GetOperand1(),
                instruction.GetMemoryAddress());
            break;
        case OpCode::StoreMemory:
            ExecuteStore(instruction, memory, registers);
            break;
        case OpCode::MoveRegister:
            ExecuteMove(instruction, registers);
            break;
        case OpCode::AddTwos:
        case OpCode::AddFloat:
        case OpCode::BitwiseOr:
        case OpCode::BitwiseAnd:
        case OpCode::BitwiseXor:
            registers.SetCell(
                instruction.GetOperand1(),
                ALU::Execute(
                    static_cast<OpCode>(opcode),
                    registers.GetCell(instruction.GetOperand2()),
                    registers.GetCell(instruction.GetOperand3())
                ));
            break;
        case OpCode::Rotate:
            registers.SetCell(
                instruction.GetOperand1(),
                ALU::Execute(
                    OpCode::Rotate,
                    registers.GetCell(instruction.GetOperand1()),
                    instruction.GetOperand3()
                ));
            break;
        case OpCode::JumpEq:
            ExecuteJump( instruction, registers );
            break;
        default:
            isHalted = true;
            break;
        }
    }
    void ExecuteLoad(Instruction instruction, Memory& memory, Registers& registers)
    {
        registers.SetCell(
            instruction.GetOperand1(),
            memory.GetCell(instruction.GetMemoryAddress()));
    }
    void ExecuteStore( Instruction instruction, Memory& memory, Registers& registers)  {
        uint8_t value = registers.GetCell(instruction.GetOperand1());
        if (instruction.GetMemoryAddress() == 0xFF)
            screen.DisplayChar(value);
        else
            memory.SetCell(instruction.GetMemoryAddress(), value);
    }
    void ExecuteMove(Instruction instruction, Registers& registers ) {
        // 40RS copies the content of register R into register s
        registers.SetCell(
            instruction.GetOperand3(),
            registers.GetCell(instruction.GetOperand2()));
    }
    void ExecuteJump(Instruction instruction, Registers& registers){
        if (registers.GetCell(instruction.GetOperand1()) == registers.GetCell(0))
            programCounter = instruction.GetMemoryAddress();
    }
};
