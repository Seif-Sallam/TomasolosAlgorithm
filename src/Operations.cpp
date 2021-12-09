#include "../headers/Operations.h"

int32_t InstructionsUnitCycles::s_CyclesCount[uint32_t(Unit::UNIT_COUNT)]{2, 2, 1, 1, 1, 2, 2, 3, 2, 10, 0};
std::string InstructionsUnitCycles::s_UnitName[uint32_t(Unit::UNIT_COUNT)] = {"LOAD", "STORE", "BEQ", "JAL", "JALR",
                                                                              "ADD", "ADDI", "NEG", "ABS", "DIV", "NAN"};
InstructionsUnitCycles InstructionsUnitCycles::s_Instance;