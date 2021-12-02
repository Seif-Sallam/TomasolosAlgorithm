#include "../headers/ReservationStation.h"
#include <iostream>
int ReservationStation::stationsCount[uint32_t(Unit::UNIT_COUNT)]{0};
ReservationStation::ReservationStation(const std::string &name, int *top, Unit type)
    : m_Type(type), Vj(0), Vk(0), Qj("N"), Qk("N"), A(0), m_IsBusy(false), m_Top(top)
{
    m_StationNumber = stationsCount[uint32_t(type)]++;
    m_UnderWorkInstruction = nullptr;
    m_Name = name + std::to_string(m_StationNumber);
}

void ReservationStation::ImGuiLayer()
{
    ImGui::Columns(8);
    {
        ImGui::Text("%s", m_Name.c_str());
        ImGui::NextColumn();
        ImGui::Text((m_IsBusy) ? "Busy" : "Not Busy");
        ImGui::NextColumn();
        ImGui::Text("%d", Vj);
        ImGui::NextColumn();
        ImGui::Text("%d", Vk);
        ImGui::NextColumn();
        ImGui::Text("%s", Qj.c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", Qk.c_str());
        ImGui::NextColumn();
        ImGui::Text("%d", A);
    }
}
void ReservationStation::FeedInstruction(Instruction *instruction)
{
    m_UnderWorkInstruction = instruction;
}

void ReservationStation::Execute(std::map<uint16_t, int16_t> &memory)
{
    // std::cout << "Executing :)";
    /*
            Essentially if the instruction we are executing is not in its last cycle
            Then we just advance :)
    */
    if (m_UnderWorkInstruction->Finished())
    {
        auto type = m_UnderWorkInstruction->type;
        switch (type)
        {
        case Unit::LW:
        {
            result = memory[A];
        }
        break;
        case Unit::SW:
        {
            result = Vk;
        }
        break;
        case Unit::ADD:
        {
            result = Vj + Vk;
        }
        break;
        case Unit::ADDI:
        {
            result = Vj + Vk;
        }
        break;
        case Unit::BEQ:
        {
            result = Vj == Vk;
        }
        break;
        case Unit::JAL:
        {
            result = m_UnderWorkInstruction->m_PC + 1;
            *m_Top = m_UnderWorkInstruction->imm + m_UnderWorkInstruction->m_PC;
        }
        break;
        case Unit::JALR:
        {
            result = m_UnderWorkInstruction->m_PC + 1;
            *m_Top = m_UnderWorkInstruction->m_PC + Vj;
        }
        break;
        case Unit::NEG:
        {
            result = ~Vj;
        }
        break;
        case Unit::ABS:
        {
            result = abs(Vj);
        }
        break;
        case Unit::DIV:
        {
            result = Vj / Vk;
        }
        break;
        default:
            result = -1;
            break;
        }
        // std::cout << "Result: " << result << std::endl;
    }
}

void ReservationStation::Clean()
{
    A = 0;
    m_InitiateExecutation = false;
    // m_UnderWorkInstruction = nullptr;
    m_IsBusy = false;
    Qj = "N";
    Qk = "N";
    Vj = 0;
    Vk = 0;
    result = 0;
}
