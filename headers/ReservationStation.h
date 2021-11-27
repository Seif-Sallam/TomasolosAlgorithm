#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "Operations.h"
#include <string>

class ReservationStation
{
public:
    ReservationStation(const std::string &name, Unit type);

    void ImGuiLayer();

private:
    std::string m_Name;
    Unit m_Type;
    uint16_t Vi, Vj, Vk, Qj, Qk, A;
    bool m_IsBusy;
};