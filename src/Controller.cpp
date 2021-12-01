#include "../headers/Controller.h"

Controller::Controller(int &top, std::vector<Instruction> &instructionsQ, std::vector<ReservationStation> &stations, RegisterFile &regFile, std::map<uint16_t, int16_t> &memory)
    : m_InstructionsQ(instructionsQ), m_Stations(stations), m_Top(top), m_RegFile(regFile), m_Memory(memory)
{
    m_CycleNumber = 0;
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
    //Issuing the instructions
    if (m_Top < m_InstructionsQ.size() && m_Top > -1)
    {
        if (m_InstructionIssuing)
        {
            Instruction &currentInst = m_InstructionsQ[m_Top];
            Unit type = currentInst.type;
            for (int i = 0; i < m_Stations.size(); i++)
            {
                if (IsCorrectUnit(m_Stations[i].GetType(), type) && !m_Stations[i].IsBusy())
                {
                    //We issue
                    m_Stations[i].FeedInstruction(&currentInst);
                    m_Stations[i].MarkBusy(true);
                    currentInst.Clean();
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
                        if (rd != 0)
                            m_RegFile.m_ProducingUnit[rd] = m_Stations[i].m_Name;
                        else
                            m_RegFile.m_ProducingUnit[rd] = "N";
                    }
                    m_WriteBackRegistersAccess.push_back(rd);
                    m_WriteBackQueues[rd].push(i);
                    m_Stations[i].m_InitiateExecutation = false;
                    currentInst.currentStage = Stage::EXECUTE;
                    m_Top++;
                    if (currentInst.type == Unit::JAL || currentInst.type == Unit::JALR)
                    {
                        //We stop any instruction from issuing
                        m_InstructionIssuing = false;
                    }
                    break;
                }
            }
        }
    }

    //Executing instructions
    for (int i = 0; i < m_Stations.size(); i++)
    {
        ReservationStation &station = m_Stations[i];
        if (station.IsBusy()) // Has an instruction inside it
        {
            Instruction &underWorkInstruction = *station.m_UnderWorkInstruction;
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
    // //Writing Back
    if (m_WriteBackRegistersAccess.size() >= 1)
    {
        int i = m_WriteBackRegistersAccess[0];
        auto &Q = m_WriteBackQueues[i];
        if (!Q.empty())
        {
            int stationNumber = Q.front();
            auto &station = m_Stations[stationNumber];
            //We are ready to write back
            if (station.m_UnderWorkInstruction->currentStage == Stage::WRITE_BACK && station.m_UnderWorkInstruction->execute.second != m_CycleNumber)
            {
                Q.pop();
                auto type = station.m_UnderWorkInstruction->type;
                if (type != Unit::BEQ && type != Unit::SW && i != 0)
                {
                    m_RegFile.m_RegisterValue[i] = station.result;
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
                    }
                }
                if (type == Unit::JAL || type == Unit::JALR)
                {
                    m_InstructionIssuing = true; // we jumped and now we want to get back to work
                }
                CDB.sourceStation = station.m_Name;
                CDB.value = station.result;

                station.m_UnderWorkInstruction->writeBack = {true, m_CycleNumber};
                station.Clean();
                m_WriteBackRegistersAccess.erase(m_WriteBackRegistersAccess.begin());
            }
        }
    }

    //Common Data Bus
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

    m_CycleNumber++;
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
        while (!m_WriteBackQueues[i].empty())
            m_WriteBackQueues[i].pop();
    }
    m_WriteBackRegistersAccess.clear();
}