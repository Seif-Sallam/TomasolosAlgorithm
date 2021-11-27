#pragma once
#include <string>
#include <queue>
#include "Operations.h"

struct Instruction
{
    std::string str;
    Unit type;
    int32_t rs1, rs2, rd, imm;
    std::pair<bool, int32_t> issue, execute, writeBack;
    Instruction(const std::string &str);
    void ImGuiLayer(bool top);

private:
    void Parse();
};