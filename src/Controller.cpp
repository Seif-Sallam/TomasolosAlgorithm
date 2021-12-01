#include "../headers/Controller.h"

Controller::Controller(int &top, std::vector<Instruction> &instructionsQ,
                       std::vector<Instruction> &instructionMemory, std::vector<ReservationStation> &stations,
                       RegisterFile &regFile, std::map<uint16_t, int16_t> &memory)
    : m_InstructionsQ(instructionsQ), m_Stations(stations), m_Top(top), m_RegFile(regFile), m_Memory(memory), m_InstructionMemory(instructionMemory)
{
    m_CycleNumber = -1;
    CDB.sourceStation = "N";
}

void Controller::JumpToCycle(int cycle)
{
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
    m_RegFile.m_ProducingUnit[0] = "N";
    m_RegFile.m_RegisterValue[0] = 0;
    //Executing instructions
    ExecuteInstructions();
    m_RegFile.m_ProducingUnit[0] = "N";
    m_RegFile.m_RegisterValue[0] = 0;
    //Writing back
    WriteBackInstructions();
    m_RegFile.m_ProducingUnit[0] = "N";
    m_RegFile.m_RegisterValue[0] = 0;

    //Common Data Bus
    CommonDataBusWork();
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
    for (int i = 0; i < 8; i++)
    {
        m_WriteBackQueues[i].clear();
    }
    m_WriteBackRegistersAccess.clear();
    m_AfterBranchInstructions.clear();
    m_BranchFound = false;
    m_InstructionIssuing = true;
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
                    currentInst.issue.first = true;
                    currentInst.issue.second = m_CycleNumber;
                    int32_t rs1, rs2, rd;
                    rs1 = currentInst.rs1;
                    rs2 = currentInst.rs2;
                    rd = currentInst.rd;
                    //  if rs1 is free
                    if (m_RegFile.m_ProducingUnit[rs1] == "N")
                    {
                        m_Stations[i].Vj = m_RegFile.m_RegisterValue[rs1];
                        m_Stations[i].Qj = "N";
                    }
                    else
                    {
                        m_Stations[i].Qj = m_RegFile.m_ProducingUnit[rs1];
                    }
                    // if res2 is also free
                    if (type == Unit::LW || type == Unit::ADDI || type == Unit::JAL)
                    {
                        m_Stations[i].Vk = currentInst.imm;
                        m_Stations[i].Qk = "N";
                    }
                    else
                    {
                        if (m_RegFile.m_ProducingUnit[rs2] == "N")
                        {
                            m_Stations[i].Vk = m_RegFile.m_RegisterValue[rs2];
                            m_Stations[i].Qk = "N";
                        }
                        else
                        {
                            m_Stations[i].Qk = m_RegFile.m_ProducingUnit[rs2];
                        }
                    }

                    if (type != Unit::SW && type != Unit::BEQ)
                    {
                        m_RegFile.m_ProducingUnit[rd] = m_Stations[i].m_Name;
                    }
                    m_WriteBackRegistersAccess.push_back(rd);
                    m_WriteBackQueues[rd].push_back({i, currentInst.m_PC});
                    m_Stations[i].m_InitiateExecutation = false;
                    currentInst.currentStage = Stage::EXECUTE;
                    m_Top++;
                    if (currentInst.type == Unit::JAL || currentInst.type == Unit::JALR)
                    {
                        //We stop any instruction from issuing
                        m_InstructionIssuing = false;
                    }
                    if (m_BranchFound == true)
                    {
                        m_AfterBranchInstructions.push_back({m_InstructionsQ.size() - 1, &currentInst});
                    }
                    if (currentInst.type == Unit::BEQ)
                    {
                        m_BranchFound = true;
                        m_BranchInstructions.push(&currentInst);
                    }
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
        if (station.IsBusy()) // Has an instruction inside it
        {
            Instruction &underWorkInstruction = *station.m_UnderWorkInstruction;

            if (!AfterBranchInstruction(underWorkInstruction))
            {
                int rs1, rs2;
                rs1 = underWorkInstruction.rs1;
                rs2 = underWorkInstruction.rs2;
                //We are executing the instruction which we have just issued
                if (underWorkInstruction.issue.second != m_CycleNumber)
                { //Instruction was NOT already executed
                    if (underWorkInstruction.currentStage == Stage::EXECUTE)
                    {
                        // Check if the operands are ready, then we initiate execuation
                        if (OperandsReady(station))
                        {
                            station.m_InitiateExecutation = true;
                        }
                        if (station.m_InitiateExecutation)
                        {

                            //If we are finished, then set that we executed the instruction and put its cycle number
                            if (underWorkInstruction.Finished())
                            {
                                underWorkInstruction.execute = {true, m_CycleNumber};
                                underWorkInstruction.currentStage = Stage::WRITE_BACK;
                            }
                            else
                            {
                                underWorkInstruction.Advance();

                                // Is it the first cycle? if yes compute the target or the address
                                if (underWorkInstruction.m_CurrentCycle == 1 && (underWorkInstruction.type == Unit::SW || underWorkInstruction.type == Unit::LW))
                                {
                                    station.A = underWorkInstruction.imm + station.Vj;
                                }
                                else if (underWorkInstruction.m_CurrentCycle == 1 && underWorkInstruction.type == Unit::BEQ)
                                {
                                    station.A = underWorkInstruction.imm;
                                }
                                else
                                    station.Execute(m_Memory);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Controller::WriteBackInstructions()
{
    if (m_WriteBackRegistersAccess.size() >= 1)
    {
        int i = m_WriteBackRegistersAccess[0];
        auto &Q = m_WriteBackQueues[i];
        if (!Q.empty())
        {
            int stationNumber = Q[0].first;
            auto &station = m_Stations[stationNumber];
            // //We are ready to write back
            if (station.m_UnderWorkInstruction->currentStage == Stage::WRITE_BACK && station.m_UnderWorkInstruction->execute.second != m_CycleNumber)
            {
                Q.erase(Q.begin());
                auto type = station.m_UnderWorkInstruction->type;
                if (type != Unit::BEQ && type != Unit::SW)
                {
                    m_RegFile.m_RegisterValue[i] = station.result;
                    if (m_RegFile.m_ProducingUnit[i] == station.m_Name)
                        m_RegFile.m_ProducingUnit[i] = "N";
                }
                else if (type == Unit::SW)
                {
                    if (m_Memory.find(station.A) == m_Memory.end())
                    {
                        m_Memory.insert({station.A, station.result});
                    }
                    else
                    {
                        m_Memory[station.A] = station.result;
                    }
                }
                if (type == Unit::BEQ)
                {
                    if (station.result == 1)
                    {
                        //Branch should be taken, we should flush the instructions after the branch and we change the PC
                        m_Top = station.m_UnderWorkInstruction->imm + station.m_UnderWorkInstruction->m_PC;
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
                        for (int index = 1; index < 8; index++)
                        {
                            auto &vec = m_WriteBackQueues[index];
                            for (int j = 0; j < vec.size(); j++)
                            {
                                auto &station = m_Stations[m_WriteBackQueues[index][j].first];
                                auto &inst = station.m_UnderWorkInstruction;
                                if (inst->m_PC > branchInst->m_PC)
                                {
                                    if (station.m_Name == m_RegFile.m_ProducingUnit[index])
                                        m_RegFile.m_ProducingUnit[index] = "N";
                                    inst->MarkAsFlushed();
                                    m_WriteBackQueues[index].erase(m_WriteBackQueues[index].begin() + j);
                                    station.Clean();
                                    j--;
                                }
                            }
                        }
                        for (int r = 0; r < m_WriteBackRegistersAccess.size(); r++)
                        {
                            int number = m_WriteBackRegistersAccess[r];
                            if (m_WriteBackQueues[number].empty())
                            {
                                m_WriteBackRegistersAccess.erase(m_WriteBackRegistersAccess.begin() + r);
                                r--;
                            }
                            else
                            {
                                auto &vec = m_WriteBackQueues[number];
                                for (int s = 0; s < vec.size(); s++)
                                {
                                    auto &st = m_Stations[vec[s].first];
                                    if (st.m_UnderWorkInstruction->m_PC > branchInst->m_PC)
                                    {
                                        vec.erase(vec.begin() + s);
                                        s--;
                                    }
                                }
                            }
                        }
                        //This is for the ADD R0 R0 R0 INSTRUCTIONS
                        for (int v = 0; v < m_Stations.size(); v++)
                        {
                            if (m_Stations[v].IsBusy())
                            {
                                if (m_Stations[v].m_UnderWorkInstruction->m_PC > branchInst->m_PC)
                                {
                                    m_Stations[v].m_UnderWorkInstruction->MarkAsFlushed();
                                    m_Stations[v].Clean();
                                }
                            }
                        }
                    }
                    m_BranchFound = !m_BranchInstructions.empty();
                    if (!m_BranchFound)
                        m_AfterBranchInstructions.clear();
                }
                if (type == Unit::JAL || type == Unit::JALR)
                {
                    m_InstructionIssuing = true; // we jumped and now we want to get back to work
                }
                CDB.sourceStation = station.m_Name;
                if (i == 0)
                    CDB.value = 0;
                else
                    CDB.value = station.result;

                station.m_UnderWorkInstruction->writeBack = {true, m_CycleNumber};
                station.Clean();
                if (!m_WriteBackRegistersAccess.empty())
                    m_WriteBackRegistersAccess.erase(m_WriteBackRegistersAccess.begin());
                // std::cout << "HERE\n";
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
