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
               std::vector<ReservationStation> &stations, RegisterFile &regFile,
               std::map<uint16_t, int16_t> &memory);
    void Advance();
    void JumpToCycle(int cycle);
    inline int32_t GetCycleNumber() { return m_CycleNumber; }
    bool IsCorrectUnit(Unit stationType, Unit instructionType);

private:
    bool OperandsReady(ReservationStation &inst);
    int32_t m_CycleNumber;
    std::vector<ReservationStation> &m_Stations;
    std::vector<Instruction> &m_InstructionsQ;
    RegisterFile &m_RegFile;
    std::map<uint16_t, int16_t> &m_Memory;
    std::array<std::queue<int32_t>, 8> m_WriteBackQueues;
    int &m_Top;

    struct CommonDataBus
    {
        std::string sourceStation;
        int16_t value;
    };
    CommonDataBus CDB;
};