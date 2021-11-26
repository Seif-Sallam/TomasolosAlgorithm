#pragma once
#include <iostream>
#include <string>
#include "../headers/ReservationStation.h"
#include "../headers/Instruction.h"
#include <vector>
#include <queue>

class Controller
{
public:
    Controller(std::queue<Instruction> &instructionsQ, std::vector<ReservationStation> &stations);

private:
    std::vector<ReservationStation> &m_Stations;
    std::queue<Instruction> &m_InstructionsQ;
};