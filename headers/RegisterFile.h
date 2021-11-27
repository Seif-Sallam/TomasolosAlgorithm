#pragma once
#include <array>
#include <string>

class RegisterFile
{
public:
    RegisterFile();

    std::array<int16_t, 8> m_RegisterValue;
    std::array<std::string, 8> m_RegisterName;
    std::array<std::string, 8> m_ProducingUnit;
};