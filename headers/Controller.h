#pragma once
#include <iostream>
#include <string>
#include "ReservationStation.h"
#include "Instruction.h"
#include <vector>

class Controller
{
public:
    Controller(std::vector<Instruction> &instructionsQ, std::vector<ReservationStation> &stations);

private:
    std::vector<ReservationStation> &m_Stations;
    std::vector<Instruction> &m_InstructionsQ;
};