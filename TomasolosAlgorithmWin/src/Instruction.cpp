#include "../headers/Instruction.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <iostream>

Instruction::Instruction(const std::string &str, int PC)
    : str(str), m_PC(PC)
{
    Parse();
    issue = {false, -1};
    execute = {false, -1};
    writeBack = {false, -1};
    m_CurrentCycle = 0;
    UpdateCycleCount();
}
Instruction::Instruction(Instruction &i2)
    : str(i2.str), type(i2.type)
{
    // std::cout << "COPYING\n";
    rs1 = i2.rs1;
    rs2 = i2.rs2;
    rd = i2.rd;
    imm = i2.imm;
    issue = i2.issue;
    execute = i2.execute;
    writeBack = i2.writeBack;
    m_MaxCycleNumber = i2.m_MaxCycleNumber;
    m_CurrentCycle = i2.m_CurrentCycle;
    currentStage = Stage::ISSUE;
    m_PC = i2.m_PC;
    Parse();
}

Instruction::Instruction(const Instruction &i2)
    : str(i2.str), type(i2.type)
{
    // std::cout << "COPYING CONST\n";
    rs1 = i2.rs1;
    rs2 = i2.rs2;
    rd = i2.rd;
    imm = i2.imm;
    issue = i2.issue;
    execute = i2.execute;
    writeBack = i2.writeBack;
    m_MaxCycleNumber = i2.m_MaxCycleNumber;
    m_CurrentCycle = i2.m_CurrentCycle;
    currentStage = Stage::ISSUE;
    m_PC = i2.m_PC;
    Parse();
}

void Instruction::Parse()
{
    int32_t index = str.find(" ");
    std::string strCopy = str;
    for (int i = 0; i < strCopy.size(); i++)
        strCopy[i] = toupper(strCopy[i]);
    std::string typeStr = strCopy.substr(0, index);
    //Removed Type
    strCopy = strCopy.substr(index + 1);

    rd = 0;
    imm = 0;
    rs1 = 0;
    rs2 = 0;
    if (typeStr == "LOAD")
    {
        type = Unit::LW;
        index = strCopy.find(",");
        rd = strCopy[1] - '0';
        strCopy = strCopy.substr(index + 1);
        int32_t index2 = strCopy.find("(");
        imm = stoi(strCopy.substr(0, index2));
        index2 = strCopy.find("R");
        rs1 = strCopy[index2 + 1] - '0';
        str = "LOAD R" + std::to_string(rd) + ", " + std::to_string(imm) + "(R" + std::to_string(rs1) + ")";
    }
    else if (typeStr == "STORE")
    {
        type = Unit::SW;
        index = strCopy.find(",");
        rs2 = strCopy[1] - '0';
        strCopy = strCopy.substr(index + 1);
        int32_t index2 = strCopy.find("(");
        imm = stoi(strCopy.substr(0, index2));
        index2 = strCopy.find("R");
        rs1 = strCopy[index2 + 1] - '0';
        str = "STORE R" + std::to_string(rs2) + ", " + std::to_string(imm) + "(R" + std::to_string(rs1) + ")";
    }
    else if (typeStr == "BEQ")
    {
        type = Unit::BEQ;
        index = strCopy.find(",");
        rs1 = strCopy[1] - '0';
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs2 = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        imm = stoi(strCopy.substr(index + 1));
        str = "BEQ R" + std::to_string(rs1) + ", " + "R" + std::to_string(rs2) + ", " + std::to_string(imm);
    }
    else if (typeStr == "JAL")
    {
        type = Unit::JAL;
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        imm = stoi(strCopy.substr(index + 1));
        str = "JAL R" + std::to_string(rd) + ", " + std::to_string(imm);
    }
    else if (typeStr == "JALR")
        type = Unit::JALR;
    else if (typeStr == "ADD")
        type = Unit::ADD;
    else if (typeStr == "ADDI")
    {
        type = Unit::ADDI;
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs1 = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        imm = stoi(strCopy);
        str = "ADDI R" + std::to_string(rd) + ", R" + std::to_string(rs1) + ", " + std::to_string(imm);
    }
    else if (typeStr == "NEG")
        type = Unit::NEG;
    else if (typeStr == "ABS")
        type = Unit::ABS;
    else if (typeStr == "DIV")
        type = Unit::DIV;

    if (type == Unit::ADD || type == Unit::DIV)
    {
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs1 = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs2 = strCopy[index + 1] - '0';
        str = ((type == Unit::ADD) ? "ADD R" : "DIV R") + std::to_string(rd) + ", R" + std::to_string(rs1) + ", R" + std::to_string(rs2);
    }

    if (type == Unit::JALR || type == Unit::NEG || type == Unit::ABS)
    {
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs1 = strCopy[index + 1] - '0';
        str = ((type == Unit::JALR) ? "JALR R" : ((type == Unit::NEG) ? "NEG R" : "ABS R")) + std::to_string(rd) + ", R" + std::to_string(rs1);
    }
}

void Instruction::ImGuiLayer(int pc, bool top, bool showTop) const
{
    ImGui::BeginChild("Instruction");
    if (showTop)
        ImGui::Columns(8);
    else
        ImGui::Columns(7);
    ImGui::PushItemWidth(20.0f);
    ImGui::Text("%d", m_PC);
    ImGui::PopItemWidth();
    ImGui::NextColumn();
    ImGui::Text(str.c_str(), "");
    ImGui::NextColumn();
    switch (type)
    {
    case Unit::ADD:
        ImGui::Text("ADD");
        break;
    case Unit::ADDI:
        ImGui::Text("ADDI");
        break;
    case Unit::LW:
        ImGui::Text("LW");
        break;
    case Unit::SW:
        ImGui::Text("SW");
        break;
    case Unit::BEQ:
        ImGui::Text("BEQ");
        break;
    case Unit::JAL:
        ImGui::Text("JAL");
        break;
    case Unit::JALR:
        ImGui::Text("JALR");
        break;
    case Unit::NEG:
        ImGui::Text("NEG");
        break;
    case Unit::ABS:
        ImGui::Text("ABS");
        break;
    case Unit::DIV:
        ImGui::Text("DIV");
        break;
    default:
        break;
    }
    ImGui::NextColumn();
    if (!m_Flushed)
    {
        ImGui::Text("%d", rs1);
        ImGui::NextColumn();
        ImGui::Text("%d", rs2);
        ImGui::NextColumn();
        ImGui::Text("%d", rd);
        ImGui::NextColumn();
        ImGui::Text("%d", imm);
        ImGui::NextColumn();
    }
    else
    {
        ImGui::TextUnformatted("FLUSHED");
        ImGui::NextColumn();
        ImGui::TextUnformatted("FLUSHED");
        ImGui::NextColumn();
        ImGui::TextUnformatted("FLUSHED");
        ImGui::NextColumn();
        ImGui::TextUnformatted("FLUSHED");
        ImGui::NextColumn();
    }
    if (showTop)
    {
        if (top)
            ImGui::Text("<-");
        else
            ImGui::Text(" ");
    }
    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::EndChild();
}

bool Instruction::Finished()
{
    return m_CurrentCycle == m_MaxCycleNumber;
}

void Instruction::Advance() { m_CurrentCycle++; }

void Instruction::Clean()
{
    currentStage = Stage::ISSUE;
    execute = {false, 0};
    issue = {false, 0};
    writeBack = {false, 0};
    m_CurrentCycle = 0;
}

void Instruction::MarkAsFlushed()
{
    m_Flushed = true;
}

bool Instruction::IsFlushed()
{
    return m_Flushed;
}