#include "../headers/RegisterFile.h"
#include <iostream>
RegisterFile::RegisterFile()
{
    for (int i = 0; i < 8; i++)
    {
        m_ProducingUnit[i].push_back("N");
        m_RegisterName[i] = "R" + std::to_string(i);
        m_RegisterValue[i] = 0;
    }
}
void RegisterFile::PopProducingUnit(const std::string &unit, int i)
{
    auto &list = m_ProducingUnit[i];
    if (list.size() == 1)
    {
        return;
    }
    else
    {
        if (unit != "N")
            list.remove(unit);
    }
}

void RegisterFile::PushProducingUnit(const std::string &unit, int i)
{
    auto &list = m_ProducingUnit[i];
    list.push_front(unit);
}