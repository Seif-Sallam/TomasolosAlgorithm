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
    std::pair<bool, int32_t> startExecute;
    Stage currentStage = ISSUE;
    int stationNumber;
    Instruction(const std::string &str, int PC);
    Instruction(Instruction &i2);
    Instruction(const Instruction &i2);
    void ImGuiLayer(int PC, bool top, bool showTop) const;
    void MarkAsFlushed();
    bool IsFlushed();
    void UpdateCycleCount();
    inline int32_t getPC() { return m_PC; }
    void Advance();
    bool Finished();
    void Clean();
    inline int32_t getMax() { return m_MaxCycleNumber; }

private:
    void Parse();
    int m_MaxCycleNumber;
    int m_CurrentCycle = 0;
    int m_PC;
    bool m_Flushed = false;
    friend class Controller;
    friend class ReservationStation;
};