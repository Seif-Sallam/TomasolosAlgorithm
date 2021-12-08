#pragma once
#include <iostream>
#include <string>
#include "ReservationStation.h"
#include "Instruction.h"
#include "RegisterFile.h"
#include <vector>
#include <stack>
class Controller
{
public:
    Controller(int &top, std::vector<Instruction> &instructionsQ,
               std::vector<Instruction> &instructionMemory, std::vector<ReservationStation> &stations,
               RegisterFile &regFile, std::map<uint16_t, int16_t> &memory);
    void Advance();
    void JumpNCycles(int n);
    inline int32_t &GetCycleNumber() { return m_CycleNumber; }
    inline int32_t GetMisPredictions() { return m_MisPredictions; }
    inline int32_t GetBranchInstructionsCount() { return m_BranchInstructionsCount; }
    inline int32_t GetNumberOfInstructions() { return m_NumberOfInstructions; }
    bool IsCorrectUnit(Unit stationType, Unit instructionType);
    void Clean();

private:
    bool OperandsReady(ReservationStation &inst);
    bool AfterBranchInstruction(Instruction &inst);
    void IssueInstructions();
    void ExecuteInstructions();
    void WriteBackInstructions();
    void CommonDataBusWork();
    bool CheckDependancy(int32_t addr, int32_t stationNumber, int32_t PC);
    bool WAWDep(Instruction &inst);

    int32_t m_CycleNumber;
    std::vector<ReservationStation> &m_Stations;
    std::vector<Instruction> &m_InstructionsQ;
    const std::vector<Instruction> &m_InstructionMemory;
    RegisterFile &m_RegFile;
    std::map<uint16_t, int16_t> &m_Memory;

    int &m_Top;
    std::vector<std::pair<int, Instruction *>> m_AfterBranchInstructions;
    std::queue<Instruction *> m_BranchInstructions;

    std::vector<std::pair<int32_t, Instruction *>> m_WritingOrder;
    bool m_BranchFound = false;
    struct CommonDataBus
    {
        std::string sourceStation;
        int16_t value;
    };
    CommonDataBus CDB;
    bool m_InstructionIssuing = true;
    int32_t m_BranchInstructionsCount = 0;
    int32_t m_MisPredictions = 0;
    int32_t m_NumberOfInstructions = 0;
};