#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "Operations.h"
#include <string>
#include "Instruction.h"
class Controller;
class ReservationStation
{
public:
    ReservationStation(const std::string &name, Unit type);

    void ImGuiLayer();
    inline std::string GetStr() { return m_Name + std::to_string(m_StationNumber); }
    static int stationsCount[uint32_t(Unit::UNIT_COUNT)];
    inline Unit GetType() { return m_Type; }
    inline bool IsBusy() { return m_IsBusy; }
    inline void MarkBusy(bool value) { m_IsBusy = value; }
    void FeedInstruction(Instruction *instruction);
    void Execute(std::map<uint16_t, int16_t> &memory);
    void Clean();

private:
    std::string m_Name;
    Unit m_Type;
    int16_t Vj, Vk, A;
    std::string Qj, Qk;
    bool m_IsBusy;
    int16_t result;
    int m_StationNumber = 0;
    Instruction *m_UnderWorkInstruction;
    bool m_InitiateExecutation = false;
    friend class Controller;
};