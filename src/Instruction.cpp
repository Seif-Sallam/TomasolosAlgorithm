#include "../headers/Instruction.h"
#include "../Thirdparty/imgui/imgui.h"
#include "../Thirdparty/imgui/imgui-SFML.h"
#include <iostream>

Instruction::Instruction(const std::string &str)
    : str(str)
{
    Parse();
}

void Instruction::Parse()
{
    int32_t index = str.find(" ");
    std::string strCopy = str;
    std::string typeStr = strCopy.substr(0, index);
    strCopy = strCopy.substr(index + 1);
    for (int i = 0; i < typeStr.size(); i++)
        typeStr[i] = toupper(typeStr[i]);
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
    }
    else if (typeStr == "JAL")
    {
        type = Unit::JAL;
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        imm = stoi(strCopy.substr(index + 1));
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
    }

    if (type == Unit::JALR || type == Unit::NEG || type == Unit::ABS)
    {
        index = strCopy.find("R");
        rd = strCopy[index + 1] - '0';
        index = strCopy.find(",");
        strCopy = strCopy.substr(index + 1);
        index = strCopy.find("R");
        rs1 = strCopy[index + 1] - '0';
    }
}

void Instruction::ImGuiLayer(bool top)
{
    ImGui::BeginChild("Instruction");
    ImGui::Columns(7);
    ImGui::Text(str.c_str());
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
    ImGui::Text("%d", rs1);
    ImGui::NextColumn();
    ImGui::Text("%d", rs2);
    ImGui::NextColumn();
    ImGui::Text("%d", rd);
    ImGui::NextColumn();
    ImGui::Text("%d", imm);
    ImGui::NextColumn();
    if (top)
        ImGui::Text("<-");
    else
        ImGui::Text("");

    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::EndChild();
}