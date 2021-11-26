#include "../headers/Application.h"
#include "../headers/Instruction.h"
int main()
{
    /*
        LOAD rd, imm(rs1)
        STORE rs2, imm(rs1)
        BEQ rs1, rs2, imm
        ADDI rd, rs1, imm
        JAL rd, imm
        JALR rd, rs1
        NEG rd, rs1
        ABS rd, rs1
        ADD rd, rs1, rs2
        DIV rd, rs1, rs2
    */
    std::string instructions[] = {
        "LOAD R2, 10(R1   )",
        "STORE    R4,      1(      R2)",
        "BEQ R7,       R5, 14",
        "JAL R3,   13",
        "ADDI    R1,  R5,   -13",
        "JALR R0,   R5",
        "NEG    R3,     R7",
        "DIV     R4, R6, R2"};
    for (int i = 0; i < sizeof(instructions) / sizeof(std::string); i++)
    {
        Instruction inst(instructions[i]);
    }
    Application app(sf::Vector2u(640, 480), "Simulation :)");
    app.Run();
}