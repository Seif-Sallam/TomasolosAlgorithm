#pragma once
#include <iostream>
#include <string>

enum class Unit
{
    LW = 0,
    SW = 1,
    BEQ = 2,
    JAL = 3,
    JALR = 4,
    ADD = 5,
    ADDI = 6,
    NEG = 7,
    ABS = 8,
    DIV = 9,
    NAN = 10,
    UNIT_COUNT,
};

class InstructionsUnitCycles
{
public:
    static int32_t s_CyclesCount[uint32_t(Unit::UNIT_COUNT)];
    static std::string s_UnitName[uint32_t(Unit::UNIT_COUNT)];

private:
    InstructionsUnitCycles() = default;
    static InstructionsUnitCycles s_Instance;
};
