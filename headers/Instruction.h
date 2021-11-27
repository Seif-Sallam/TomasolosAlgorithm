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
    Instruction(const std::string &str);
    void ImGuiLayer(bool top);
    inline void SetMaxCycles(int cycles)
    {
        m_MaxCycleNumber = cycles;
        if (type == Unit::JAL || type == Unit::JALR || type == Unit::BEQ ||
            type == Unit::SW || type == Unit::LW)
            m_MaxCycleNumber++;
    }
    void Advance();
    bool Finished();

private:
    void
    Parse();
    int m_MaxCycleNumber;
    int m_CurrentCycle = 0;
    friend class Controller;
};