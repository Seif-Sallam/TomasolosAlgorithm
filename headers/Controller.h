#pragma once
#include <iostream>
#include <string>
#include "ReservationStation.h"
#include "Instruction.h"
#include "RegisterFile.h"
#include <vector>

class Controller
{
public:
    Controller(int &top, std::vector<Instruction> &instructionsQ,
               std::vector<Instruction> &instructionMemory, std::vector<ReservationStation> &stations,
               RegisterFile &regFile, std::map<uint16_t, int16_t> &memory);
    void Advance();
    void JumpToCycle(int cycle);
    inline int32_t &GetCycleNumber() { return m_CycleNumber; }
    bool IsCorrectUnit(Unit stationType, Unit instructionType);
    void Clean();

private:
    bool OperandsReady(ReservationStation &inst);
    bool AfterBranchInstruction(Instruction &inst);
    int32_t m_CycleNumber;
    std::vector<ReservationStation> &m_Stations;
    std::vector<Instruction> &m_InstructionsQ;
    const std::vector<Instruction> &m_InstructionMemory;
    RegisterFile &m_RegFile;
    std::map<uint16_t, int16_t> &m_Memory;
    std::array<std::queue<int32_t>, 8> m_WriteBackQueues;
    int &m_Top;
    std::vector<int> m_WriteBackRegistersAccess;
    std::vector<Instruction *> m_AfterBranchInstructions;
    std::queue<Instruction *> m_BranchInstructions;
    bool m_BranchFound = false;
    struct CommonDataBus
    {
        std::string sourceStation;
        int16_t value;
    };
    CommonDataBus CDB;
    bool m_InstructionIssuing = true;
};