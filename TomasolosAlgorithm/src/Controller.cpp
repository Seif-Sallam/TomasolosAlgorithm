#include "../headers/Controller.h"

Controller::Controller(int &top, std::vector<Instruction> &instructionsQ,
                       std::vector<Instruction> &instructionMemory, std::vector<ReservationStation> &stations,
                       RegisterFile &regFile, std::map<uint16_t, int16_t> &memory)
    : m_InstructionsQ(instructionsQ), m_Stations(stations), m_Top(top), m_RegFile(regFile), m_Memory(memory), m_InstructionMemory(instructionMemory)
{
    m_CycleNumber = -1;
    CDB.sourceStation = "N";
}

void Controller::JumpNCycles(int n)
{
    for (int i = 0; i < n; i++)
        Advance();
}

/*
    - We update every reseravtion station with its own working unit
    - We issue only ONE instruction if and only if there is a free reservatation station
    - We should have a common data bus
*/

void Controller::Advance()
{
    m_CycleNumber++;
    //Issuing the instructions
    IssueInstructions();
    CleanR0();

    //Writing back
    WriteBackInstructions();
    CleanR0();

    //Common Data Bus
    CommonDataBusWork();

    ExecuteInstructions();
    CleanR0();
}
bool Controller::OperandsReady(ReservationStation &station)
{
    auto type = station.GetType();
    bool flag = true;
    if (type == Unit::LW || type == Unit::SW || type == Unit::ADDI ||
        type == Unit::BEQ || type == Unit::ADD ||
        type == Unit::JALR || type == Unit::NEG ||
        type == Unit::ABS || type == Unit::DIV)
    {
        flag = station.Qj == "N";
    }
    if (type == Unit::SW || type == Unit::BEQ ||
        type == Unit::ADD || type == Unit::DIV)
    {
        flag = flag && station.Qk == "N";
    }

    if (type == Unit::SW || type == Unit::LW)
    {
        //Check that the address computed is not The same as the others
    }

    return flag;
}

bool Controller::IsCorrectUnit(Unit stationType, Unit instructionType)
{
    if (stationType == Unit::ADD && instructionType == Unit::ADDI)
        return true;
    if (stationType == Unit::JAL && instructionType == Unit::JALR)
        return true;

    return stationType == instructionType;
}

void Controller::Clean()
{
    m_AfterBranchInstructions.clear();
    while (m_BranchInstructions.empty() == false)
        m_BranchInstructions.pop();
    m_BranchFound = false;
    m_InstructionIssuing = true;
    m_BranchInstructionsCount = 0;
    m_MisPredictions = 0;
    m_NumberOfInstructions = 0;
}

bool Controller::AfterBranchInstruction(Instruction &inst)
{
    for (int i = 0; i < m_AfterBranchInstructions.size(); i++)
        if (inst.m_PC == m_AfterBranchInstructions[i].second->m_PC)
            return true;
    return false;
}

void Controller::IssueInstructions()
{
    if (m_Top < m_InstructionMemory.size() && m_Top > -1)
    {
        if (m_InstructionIssuing)
        {
            Unit type = m_InstructionMemory[m_Top].type;
            for (int i = 0; i < m_Stations.size(); i++)
            {
                if (IsCorrectUnit(m_Stations[i].GetType(), type) && !m_Stations[i].IsBusy())
                {
                    //We issue
                    m_InstructionsQ.push_back(m_InstructionMemory[m_Top]);
                    Instruction &currentInst = m_InstructionsQ.back();
                    m_Stations[i].FeedInstruction(&currentInst);
                    m_Stations[i].MarkBusy(true);
                    currentInst.stationNumber = i;
                    currentInst.stationNumber = i;
                    currentInst.issue.first = true;
                    currentInst.issue.second = m_CycleNumber;
                    int32_t rs1, rs2, rd;
                    rs1 = currentInst.rs1;
                    rs2 = currentInst.rs2;
                    rd = currentInst.rd;
                    //  if rs1 is free
                    if (m_RegFile.m_ProducingUnit[rs1].front() == "N")
                    {
                        m_Stations[i].Vj = m_RegFile.m_RegisterValue[rs1];
                        m_Stations[i].Qj = "N";
                    }
                    else
                    {
                        m_Stations[i].Qj = m_RegFile.m_ProducingUnit[rs1].front();
                    }
                    if (type == Unit::LW || type == Unit::ADDI || type == Unit::JAL)
                    {
                        m_Stations[i].Vk = currentInst.imm;
                        m_Stations[i].Qk = "N";
                    }
                    else
                    {
                        // if rs2 is also free
                        if (m_RegFile.m_ProducingUnit[rs2].front() == "N")
                        {
                            m_Stations[i].Vk = m_RegFile.m_RegisterValue[rs2];
                            m_Stations[i].Qk = "N";
                        }
                        else
                        {
                            m_Stations[i].Qk = m_RegFile.m_ProducingUnit[rs2].front();
                        }
                    }

                    if (type != Unit::SW && type != Unit::BEQ)
                    {
                        m_RegFile.PushProducingUnit(m_Stations[i].m_Name, rd);
                    }
                    m_Stations[i].m_InitiateExecution = false;
                    currentInst.currentStage = Stage::EXECUTE;
                    m_Top++;
                    if (currentInst.type == Unit::JAL || currentInst.type == Unit::JALR)
                        //We stop any instruction from issuing
                        m_InstructionIssuing = false;
                    if (m_BranchFound == true)
                        m_AfterBranchInstructions.push_back({m_InstructionsQ.size() - 1, &currentInst});

                    if (currentInst.type == Unit::BEQ)
                    {
                        m_BranchFound = true;
                        m_BranchInstructions.push(&currentInst);
                    }

                    m_WritingOrder.push_back({i, &currentInst});
                    break;
                }
            }
        }
    }
}

void Controller::ExecuteInstructions()
{
    for (int i = 0; i < m_Stations.size(); i++)
    {
        ReservationStation &station = m_Stations[i];
        if (station.GetType() == Unit::LW || station.GetType() == Unit::SW)
        {
            if (station.IsBusy())
            {
                Instruction &inst = *station.m_UnderWorkInstruction;
                if (!AfterBranchInstruction(inst) && inst.issue.second != m_CycleNumber && inst.currentStage == Stage::EXECUTE)
                {

                    if (station.Qj == "N")
                    {
                        if (inst.m_CurrentCycle <= 1)
                        {
                            inst.Advance();
                            if (inst.m_CurrentCycle == 2)
                            {
                                station.A = inst.imm + station.Vj;
                                inst.startExecute = {true, m_CycleNumber};
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < m_Stations.size(); i++)
    {
        ReservationStation &station = m_Stations[i];
        if (station.IsBusy()) // Has an instruction inside it
        {
            Instruction &currentInst = *station.m_UnderWorkInstruction;

            if (!AfterBranchInstruction(currentInst) && (currentInst.issue.second != m_CycleNumber) && (currentInst.currentStage == Stage::EXECUTE))
            {
                // Check if the operands are ready, then we initiate execuation
                if (OperandsReady(station))
                {
                    if (station.m_Type != Unit::SW && station.m_Type != Unit::LW)
                        station.m_InitiateExecution = true;
                    else
                    {
                        if (currentInst.m_CurrentCycle == 2)
                        {
                            if (!CheckDependancy(station.A, i, currentInst.m_PC))
                            {
                                station.m_InitiateExecution = true;
                            }
                        }
                    }
                }
                if (station.m_InitiateExecution)
                {
                    if (currentInst.m_CurrentCycle == 0)
                    {
                        currentInst.startExecute = {true, m_CycleNumber};
                    }
                    //If we are finished, then set that we executed the instruction and put its cycle number
                    if (!((station.GetType() == Unit::LW || station.GetType() == Unit::SW) && currentInst.startExecute.second == m_CycleNumber))
                        currentInst.Advance();
                    // Is it the first cycle? if yes compute the target or the address
                    if (currentInst.m_CurrentCycle == 1 && currentInst.type == Unit::BEQ)
                    {
                        station.A = currentInst.imm + m_Top + 1;
                    }
                    else
                        station.Execute(m_Memory);
                    if (currentInst.Finished())
                    {
                        currentInst.execute = {true, m_CycleNumber};
                        currentInst.currentStage = Stage::WRITE_BACK;
                    }
                }
            }
        }
    }
}

void Controller::WriteBackInstructions()
{
    for (int order = 0; order < m_WritingOrder.size(); order++)
    {
        int st = m_WritingOrder[order].first;
        if (m_Stations[st].IsBusy())
        {
            Instruction &currentInst = *m_Stations[st].m_UnderWorkInstruction;
            auto &station = m_Stations[st];
            if (currentInst.currentStage == Stage::WRITE_BACK)
            {
                bool dep = WAWDep(currentInst);
                if (!dep)
                {
                    // We actually start writing back
                    auto type = currentInst.type;
                    if (type != Unit::BEQ && type != Unit::SW)
                    {
                        int rd = currentInst.rd;
                        m_RegFile.m_RegisterValue[rd] = station.result;
                        m_RegFile.PopProducingUnit(station.m_Name, rd);
                    }
                    else if (type == Unit::SW)
                    {
                        if (m_Memory.find(station.A) == m_Memory.end())
                            m_Memory.insert({station.A, station.result});
                        else
                            m_Memory[station.A] = station.result;
                    }
                    if (type == Unit::BEQ)
                    {
                        m_BranchInstructionsCount++;
                        if (station.result == 1)
                        {
                            m_MisPredictions++;
                            //Branch should be taken, we should flush the instructions after the branch and we change the PC
                            m_Top = station.m_UnderWorkInstruction->imm + station.m_UnderWorkInstruction->m_PC + 1;
                            //Flush?
                            Instruction *branchInst = m_BranchInstructions.front();
                            m_BranchInstructions.pop();
                            //Clean those instruction from the write back queues
                            while (!m_BranchInstructions.empty() && m_BranchInstructions.front()->m_PC > branchInst->m_PC)
                            {
                                m_BranchInstructions.front()->MarkAsFlushed();
                                m_Stations[m_BranchInstructions.front()->stationNumber].Clean();
                                m_BranchInstructions.pop();
                            }
                            for (int v = 0; v < m_Stations.size(); v++)
                            {
                                if (m_Stations[v].IsBusy())
                                {
                                    if (m_Stations[v].m_UnderWorkInstruction->m_PC > branchInst->m_PC)
                                    {
                                        int rd = m_Stations[v].m_UnderWorkInstruction->rd;
                                        m_RegFile.PopProducingUnit(m_Stations[v].m_Name, rd);
                                        m_Stations[v].m_UnderWorkInstruction->MarkAsFlushed();
                                        m_Stations[v].Clean();
                                    }
                                }
                            }
                            for (int i = 0; i < m_AfterBranchInstructions.size(); i++)
                            {
                                auto &inst = m_AfterBranchInstructions[i].second;
                                if (inst->m_PC > currentInst.m_PC)
                                {
                                    if (inst->type == Unit::JAL || inst->type == Unit::JALR)
                                    {
                                        m_InstructionIssuing = true;
                                    }
                                    m_AfterBranchInstructions.erase(m_AfterBranchInstructions.begin() + i);
                                    i--;
                                }
                            }
                            for (int i = 0; i < m_WritingOrder.size(); i++)
                            {
                                if (currentInst.m_PC < m_WritingOrder[i].second->getPC())
                                {
                                    m_WritingOrder.erase(m_WritingOrder.begin() + i);
                                    i--;
                                }
                            }
                        }
                        else
                        {
                            m_BranchInstructions.pop();
                            for (int i = 0; i < m_AfterBranchInstructions.size(); i++)
                            {
                                auto &inst = m_AfterBranchInstructions[i].second;
                                if (inst->type == Unit::BEQ)
                                {
                                    m_AfterBranchInstructions.erase(m_AfterBranchInstructions.begin() + i);
                                    i--;
                                    break;
                                }
                                else
                                {
                                    if (inst->m_PC > currentInst.m_PC)
                                    {
                                        m_AfterBranchInstructions.erase(m_AfterBranchInstructions.begin() + i);
                                        i--;
                                    }
                                }
                            }
                        }
                        m_BranchFound = !m_BranchInstructions.empty();
                    }
                    if (type == Unit::JAL || type == Unit::JALR)
                    {
                        m_InstructionIssuing = true; // we jumped and now we want to get back to work
                    }
                    CDB.sourceStation = station.m_Name;
                    if (currentInst.rd == 0)
                    {
                        CDB.value = 0;
                    }
                    else
                        CDB.value = station.result;

                    station.m_UnderWorkInstruction->writeBack = {true, m_CycleNumber};
                    station.Clean();
                    m_NumberOfInstructions++;
                    m_WritingOrder.erase(m_WritingOrder.begin() + order);
                    m_LastInstructionWrote = m_CycleNumber;
                    break;
                }
            }
        }
    }
}

void Controller::CommonDataBusWork()
{
    if (CDB.sourceStation != "N")
    {
        for (int i = 0; i < m_Stations.size(); i++)
        {
            auto &station = m_Stations[i];
            if (station.Qj == CDB.sourceStation)
            {
                station.Qj = "N";
                station.Vj = CDB.value;
            }
            if (station.Qk == CDB.sourceStation)
            {
                station.Qk = "N";
                station.Vk = CDB.value;
            }
        }
        CDB.sourceStation = "N";
    }
}

bool Controller::CheckDependancy(int32_t addr, int32_t stationNumber, int32_t PC)
{
    auto myType = m_Stations[stationNumber].GetType();
    for (int i = 0; i < m_Stations.size(); i++)
    {
        if (myType == Unit::SW)
        {
            if (m_Stations[i].IsBusy() && (m_Stations[i].GetType() == Unit::SW || m_Stations[i].GetType() == Unit::LW))
                if (stationNumber != i) // we are not at the same station
                {
                    auto &station = m_Stations[i];
                    if (station.A == addr)
                    {
                        if (station.m_UnderWorkInstruction->m_PC < PC)
                            return true;
                    }
                }
        }
        else if (myType == Unit::LW)
        {
            if (m_Stations[i].IsBusy() && (m_Stations[i].GetType() == Unit::SW))
                if (stationNumber != i) // we are not at the same station
                {
                    auto &station = m_Stations[i];
                    if (station.A == addr)
                    {
                        if (station.m_UnderWorkInstruction->m_PC < PC)
                            return true;
                    }
                }
        }
    }
    return false;
}

bool Controller::WAWDep(Instruction &inst)
{
    int stationNumber = inst.stationNumber;
    int &instPC = inst.m_PC;
    for (int i = 0; i < m_Stations.size(); i++)
    {
        auto &currentStation = m_Stations[i];
        if (currentStation.IsBusy())
        {
            int rd = currentStation.m_UnderWorkInstruction->rd;
            if (rd != 0)
            {
                if (rd == inst.rd)
                {
                    if (instPC > currentStation.m_UnderWorkInstruction->m_PC)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Controller::CleanR0()
{
    while (m_RegFile.m_ProducingUnit[0].front() != "N")
        m_RegFile.m_ProducingUnit[0].pop_front();
    m_RegFile.m_RegisterValue[0] = 0;
}

int32_t Controller::GetLastInstructionWrote()
{
    return m_LastInstructionWrote;
}