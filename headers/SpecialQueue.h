#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Instruction.h"

struct SpecialQueue
{

    std::vector<Instruction> container;
    int top = -1;
    inline Instruction &operator[](int index)
    {
        return container[index];
    }
    inline void Push(Instruction &&i)
    {
        container.push_back(i);
        top = 0;
    }
    inline void Clear()
    {
        container.clear();
        top = -1;
    }
    inline Instruction Pop()
    {
        if (top > -1)
        {
            Instruction i = container[top];
            container.pop_back();
            top += 1;
            return i;
        }
        else
        {
            return Instruction("ADD R0, R0, R0"); // NOP
        }
    }
};