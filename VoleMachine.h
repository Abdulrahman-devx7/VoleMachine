#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include "Screen.h"
#include "CPU.h"
#include "Memory.h"
#include <iomanip>

using namespace std;

class VoleMachine {
private:
    enum enMainMenuOptions {
        eLoadProgram = 1,
        eRunProgram = 2,
        eOverwriteMemory = 3,
        eOverwriteRegister = 4,
        eChooseMode = 5,
        eExit = 6
    };

    enum enExecutionMode {
        eStepByStep = 1,
        eAllAtOnce = 2
    };

    Screen _outputScreen;
    CPU _CPU;
    Memory _memory;

    enExecutionMode _currentMode = enExecutionMode::eStepByStep;

    static bool _isValidVoleInstruction(const std::string& instr)
    {
        const std::regex volePattern(
            "^([1-3][0-9a-fA-F]{3}|40[0-9a-fA-F]{2}|[5-9][0-9a-fA-F]{3}|[Aa][0-9a-fA-F]0[0-9a-fA-F]|[Bb][0-9a-fA-F]{3}|[Cc]000)$");

        return std::regex_match(instr, volePattern);
    }

    static bool _isValidHexAddress(const std::string& str) 
    {
        const std::regex hexPattern("^[0-9a-fA-F]{1,2}$");
        return std::regex_match(str, hexPattern);
    }

    static void ResetScreen()
    {
        system("cls");
        system("color 0f");
    }

    void _displayMachineState()
    {
        std::cout << "\n========== MACHINE STATE ==========\n";
        std::cout << "\nPC: " << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)_CPU.GetCU().GetPC();
        std::cout << "\nIR: " << std::setw(4) << _CPU.GetCU().GetIR();
        std::cout << "\n\nREGISTERS:\n";
        for (int i = 0; i < 16; i++) {
            std::cout << "R" << std::hex << i << ": " << std::setw(2) << (int)_CPU.GetRegister().GetCell(i) << "  ";
            if (i % 4 == 3) std::cout << "\n";
        }
        std::cout << "\nMEMORY:\n";
        for (int i = 0; i < 256; i++) {
            if (i % 16 == 0) std::cout << "\n" << std::setw(2) << i << ": ";
            std::cout << std::setw(2) << (int)_memory.GetCell(i) << " ";
        }
        std::cout << "\n\nSCREEN OUTPUT:\n";
        std::cout << _outputScreen.GetOutput();
        std::cout << "\n\nSCREEN OUTPUT (HEX):\n";
        for (unsigned char c : _outputScreen.GetOutput())
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)c << " ";
        std::cout << "\n===================================\n";
        std::cout << std::dec;
    }

    void _ShowLoadProgramScreen() 
    {
        cout << "===========================================\n";
        cout << "\t   Load Program Screen\n";
        cout << "===========================================\n";

        bool isLoadingError = false;
        bool isFileEmpty = false;

        string fileName;

        cout << "Enter program file name (e.g., program.txt): \n";
        std::cin >> fileName;

        fstream programFile;
        programFile.open(fileName, ios::in);

        string loadingAddress;
        cout << "\nPlease, enter the address you want to load the program into (it must be an even number)\n";
        cin >> loadingAddress;

        uint8_t oldPC = _CPU.GetCU().GetPC();

        if (!_isValidHexAddress(loadingAddress))
        {
            cout << "\nPlease, enter a valid hexadecimal address!\n";
            return;
        }
        else if ((stoi(loadingAddress, nullptr, 16) % 2) != 0)
        {
            cout << "\nPlease, ensure that you've entered an even number for the address (i.e., 00, 02, 04,...)\n";
            return;
        }

        if (programFile.is_open())
        {
            string instruction = "";

            // We'll use this to ensure the file isn't empty
            int NumInstructions = 0;

            while (getline(programFile, instruction))
            {
                if (!instruction.empty() && instruction.back() == '\r')
                    instruction.pop_back();

                if (instruction.empty())
                    continue;

                else if (!_isValidVoleInstruction(instruction))
                {
                    isLoadingError = true;
                    break;
                }
                else
                    NumInstructions++;
            }

            if (isLoadingError)
            {
                cout << "\n Something went wrong with this file!\nplease ensure that it correctly consists of correct vole machine instructions put back to back in the .txt file (i.e. 2332) \n";
                _CPU.GetCU().SetPC(oldPC);
                return;
            }
            else if (NumInstructions == 0)
            {
                cout << "\nPlease, enter a non-empty file!\n";
                _CPU.GetCU().SetPC(oldPC);
                return;
            }
            else if (((NumInstructions * 2) + _CPU.GetCU().GetPC()) > 256)
            {
                cout << "\nThe program is too long and goes out of the memory's bounds.\nPlease, make sure that your program fits in memory. Account for the size of your program and where are you starting to put it in memory \n";
                _CPU.GetCU().SetPC(oldPC);
                return;
            }

            // These two functions reset the file I/O flags and put the reading cursor back to the beginning 
            // such that the second while loop can start working and actually load the program.
            programFile.clear();
            programFile.seekg(0);

            _CPU.Reset();
            _memory.Reset();

            _CPU.GetCU().SetPC(stoi(loadingAddress, nullptr, 16));

            int PC = stoi(loadingAddress, nullptr, 16);
            while (getline(programFile, instruction))
            {
                if (!instruction.empty() && instruction.back() == '\r')
                    instruction.pop_back();

                if (instruction.empty()) // 
                    continue;

                string firstTwo = instruction.substr(0, 2);
                _memory.SetCell(PC, stoi(firstTwo, nullptr, 16));

                PC++;

                string lastTwo = instruction.substr(2, 2);
                _memory.SetCell(PC, stoi(lastTwo, nullptr, 16));

                PC++;
            }

            _CPU.setHaltStatus(false);
        }
        else
        {
            _CPU.GetCU().SetPC(oldPC);
            cout << "\nFile doesn't exist! Ensure that you've' typed a correct file name (i.e., program.txt)\n";
        }

        programFile.close();
    }

    void _ShowOverwriteMemoryScreen()
    {
        cout << "\nPlease, enter the memory address you'd like to overrwrite in hexadecimal (00-FF): \n";
        string memoryAdress;

        cin >> memoryAdress;

        if (!_isValidHexAddress(memoryAdress) || stoi(memoryAdress, nullptr, 16) > 255)
            cout << "\nPlease, enter a valid hexadecimal memory address!(i.e., a hexadecimal number from 00-FF)\n";
        else
        {
            cout << "\nPlease, enter the content you'd like to put (i.e., a hexadecimal number from 00-FF)\n";
            string content;
            cin >> content;

            if (!_isValidHexAddress(content))
                cout << "\nPlease, enter a valid hexadecimal memory address!\n";
            else
                _memory.SetCell(stoi(memoryAdress, nullptr, 16), stoi(content, nullptr, 16));
        }
    }

    void _ShowOverwriteRegisterScreen()
    {
        cout << "\nPlease, enter the memory address you'd like to overrwrite in hexadecimal (00-0F): \n";
        string memoryAdress;

        cin >> memoryAdress;

        if (!_isValidHexAddress(memoryAdress) || stoi(memoryAdress, nullptr, 16) > 15)
            cout << "\nPlease, enter a valid hexadecimal memory address! (00-0F)\n";
        else
        {
            cout << "\nPlease, enter the content you'd like to put (i.e., a hexadecimal number from 00-0F)\n";
            string content;
            cin >> content;

            if (!_isValidHexAddress(content))
                cout << "\nPlease, enter a valid hexadecimal memory address!\n";
            else
                _CPU.GetRegister().SetCell(stoi(memoryAdress, nullptr, 16), stoi(content, nullptr, 16));
        }
    }

    void _ShowChooseModeScreen() 
    {
        cout << "Press 1 for the step-by-step mode and 2 to run all at once";
        short choice = 0;

        cin >> choice;

        if (choice < 1 || choice>2)
            cout << "Please, pick a valid choice!";
        else
            _currentMode = enExecutionMode(choice);
    }

    void _ShowRunProgramScreen() 
    {
        if (_CPU.IsHalted())
        {
            cout << "Please, make sure that you've loaded a program or that the current program hasn't halted!\n PC: 0x"
                << uppercase << hex << setw(2) << setfill('0') << (int)_CPU.GetCU().GetPC() << dec << "\n";
            cout << "IR: 0x"
                << uppercase << hex << setw(4) << setfill('0') << (int)_CPU.GetCU().GetIR() << dec << "\n";
        }
        else
        {
            _CPU.Start();
            switch (_currentMode)
            {
            case enExecutionMode::eStepByStep:
            {
                while (_CPU.IsHalted() == false)
                {
                    _CPU.Step(_memory);
                    _displayMachineState();
                    _PromptUser();
                    ResetScreen();
                }
                break;
            }
            case enExecutionMode::eAllAtOnce:
            {
                _CPU.Run(_memory);
                _displayMachineState();
                _PromptUser();
            }
            }
        }
    }

    void _PromptUser() 
    {
        cout << "\n\nPress any key to continue...\n";
        system("pause>0");
    }

    void _PerformMainMenuOption(enMainMenuOptions option) 
    {
        switch (option) 
        {
        case enMainMenuOptions::eLoadProgram:
            system("cls");
            _ShowLoadProgramScreen();
            _PromptUser();
            break;
        case enMainMenuOptions::eRunProgram:
            system("cls");
            _ShowRunProgramScreen();
            _PromptUser();
            break;
        case enMainMenuOptions::eOverwriteMemory:
            system("cls");
            _ShowOverwriteMemoryScreen();
            _PromptUser();
            break;
        case enMainMenuOptions::eOverwriteRegister:
            system("cls");
            _ShowOverwriteRegisterScreen();
            _PromptUser();
            break;
        case enMainMenuOptions::eChooseMode:
            system("cls");
            _ShowChooseModeScreen();
            _PromptUser();
            break;
        case enMainMenuOptions::eExit:
            break;
        }
    }

    enMainMenuOptions _ReadMainMenuOption() {
        short choice;
        cout << "Choose what do you want to do? [1 to 6]? ";
        cin >> choice;
        return (enMainMenuOptions)choice;
    }

public:
    VoleMachine() : _outputScreen(), _CPU(_outputScreen), _memory() {}

    void Start() 
    {
        enMainMenuOptions choice;
        do { 
            system("cls");
            cout << "===========================================\n";
            cout << "\t\tMain Menu\n";
            cout << "===========================================\n";
            cout << "\t[1] Load a program.\n";
            cout << "\t[2] Run a program.\n";
            cout << "\t[3] Overwrite memory content.\n";
            cout << "\t[4] Overwrite registers content.\n";
            cout << "\t[5] Choose execution mode.\n";
            cout << "\t[6] Exit.\n";
            cout << "===========================================\n";

            choice = _ReadMainMenuOption();
            _PerformMainMenuOption(choice);

        } while (choice != enMainMenuOptions::eExit);
    }
};

    bool LoadProgramFile(const std::string& fileName, uint8_t address, std::string& error)
    {
        std::ifstream programFile(fileName);
        if (!programFile.is_open()) { error = "Could not open program file."; return false; }
        std::vector<std::string> instructions;
        std::string instruction;
        while (std::getline(programFile, instruction)) {
            if (!instruction.empty() && instruction.back() == '\r') instruction.pop_back();
            if (instruction.empty()) continue;
            if (!_isValidVoleInstruction(instruction)) { error = "Invalid instruction: " + instruction; return false; }
            instructions.push_back(instruction);
        }
        if (instructions.empty()) { error = "The program file is empty."; return false; }
        if (static_cast<unsigned>(address) + instructions.size() * 2 > 256) { error = "The program does not fit in memory at that address."; return false; }
        _CPU.Reset();
        _memory.Reset();
        _CPU.GetCU().SetPC(address);
        unsigned pc = address;
        for (const auto& item : instructions) {
            _memory.SetCell(static_cast<uint8_t>(pc++), static_cast<uint8_t>(std::stoul(item.substr(0, 2), nullptr, 16)));
            _memory.SetCell(static_cast<uint8_t>(pc++), static_cast<uint8_t>(std::stoul(item.substr(2, 2), nullptr, 16)));
        }
        _CPU.setHaltStatus(false);
        error.clear();
        return true;
    }

    void BeginExecution() { _CPU.Start(); }
    void ExecuteOneStep() { _CPU.Step(_memory); }
    void ResetMachine() { _CPU.Reset(); _memory.Reset(); }
    bool IsHalted() const { return _CPU.IsHalted(); }
    uint8_t GetPC() { return _CPU.GetCU().GetPC(); }
    uint16_t GetIR() { return _CPU.GetCU().GetIR(); }
    uint8_t GetRegisterValue(int index) { return _CPU.GetRegister().GetCell(static_cast<uint8_t>(index)); }
    uint8_t GetMemoryValue(int address) const { return _memory.GetCell(static_cast<uint8_t>(address)); }
    std::string GetScreenOutput() const { return _outputScreen.GetOutput(); }

    void Start() 
    {
        enMainMenuOptions choice;
        do { 
            system("cls");
            cout << "===========================================\n";
            cout << "\t\tMain Menu\n";
            cout << "===========================================\n";
            cout << "\t[1] Load a program.\n";
            cout << "\t[2] Run a program.\n";
            cout << "\t[3] Overwrite memory content.\n";
            cout << "\t[4] Overwrite registers content.\n";
            cout << "\t[5] Choose execution mode.\n";
            cout << "\t[6] Exit.\n";
            cout << "===========================================\n";

            choice = _ReadMainMenuOption();
            _PerformMainMenuOption(choice);

        } while (choice != enMainMenuOptions::eExit);
    }
};
