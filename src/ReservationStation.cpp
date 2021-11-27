#include "../headers/ReservationStation.h"
int ReservationStation::stationsCount[uint32_t(Unit::UNIT_COUNT)]{0};
ReservationStation::ReservationStation(const std::string &name, Unit type)
    : m_Name(name), m_Type(type), Vi(0), Vj(0), Vk(0), Qj(0), Qk(0), A(0), m_IsBusy(false)
{
    m_StationNumber = stationsCount[uint32_t(type)]++;
}

void ReservationStation::ImGuiLayer()
{
    ImGui::Columns(8);
    {
        ImGui::Text("%s %d", m_Name.c_str(), m_StationNumber);
        ImGui::NextColumn();
        ImGui::Text((m_IsBusy) ? "Busy" : "Not Busy");
        ImGui::NextColumn();
        ImGui::Text("%d", Vi);
        ImGui::NextColumn();
        ImGui::Text("%d", Vj);
        ImGui::NextColumn();
        ImGui::Text("%d", Vk);
        ImGui::NextColumn();
        ImGui::Text("%d", Qj);
        ImGui::NextColumn();
        ImGui::Text("%d", Qk);
        ImGui::NextColumn();
        ImGui::Text("%d", A);
    }
    ImGui::Columns(1);
}