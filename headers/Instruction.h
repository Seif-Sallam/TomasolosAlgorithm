#pragma once
#include <string>
#include <queue>
#include <iostream>
#include "Operations.h"
enum Stage
{
    ISSUE,
    EXECUTE,
    WRITE_BACK
};

struct Instruction
{
    std::string str;
    Unit type;
    int32_t rs1, rs2, rd, imm;
    std::pair<bool, int32_t> issue, execute, writeBack;
    Stage currentStage = ISSUE;
    Instruction(const std::string &str, int PC);
    Instruction(Instruction &i2);
    Instruction(const Instruction &i2);
    void ImGuiLayer(bool top) const;
    inline void UpdateCycleCount()
    {
        m_MaxCycleNumber = s_CyclesCount[(int)(type)];
        if (type == Unit::JAL || type == Unit::JALR || type == Unit::BEQ)
            m_MaxCycleNumber++;
        if (type == Unit::SW || type == Unit::LW)
            m_MaxCycleNumber += 2;
    }
    void Advance();
    bool Finished();
    void Clean();

private:
    void Parse();
    int m_MaxCycleNumber;
    int m_CurrentCycle = 0;
    int m_PC;
    friend class Controller;
    friend class ReservationStation;
};