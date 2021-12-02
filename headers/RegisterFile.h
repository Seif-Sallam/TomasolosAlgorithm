#pragma once
#include <array>
#include <string>
#include <list>
class RegisterFile
{
public:
    RegisterFile();

    std::array<int16_t, 8> m_RegisterValue;
    std::array<std::string, 8> m_RegisterName;
    std::array<std::list<std::string>, 8> m_ProducingUnit;
    void PopProducingUnit(const std::string &unit, int i);
    void PushProducingUnit(const std::string &unit, int i);
};