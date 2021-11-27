#include "../headers/RegisterFile.h"

RegisterFile::RegisterFile()
{
    for (int i = 0; i < 8; i++)
    {
        m_ProducingUnit[i] = "N";
        m_RegisterName[i] = "R" + std::to_string(i);
        m_RegisterValue[i] = 0;
    }
}