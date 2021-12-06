#pragma once

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
    UNIT_COUNT
};

static int32_t s_CyclesCount[uint32_t(Unit::UNIT_COUNT)]{2, 2, 1, 1, 1, 2, 2, 3, 2, 10};
static std::string s_UnitName[uint32_t(Unit::UNIT_COUNT)] = {"LW", "SW", "BEQ", "JAL", "JALR",
                                                             "ADD", "ADDI", "NEG", "ABS", "DIV"};