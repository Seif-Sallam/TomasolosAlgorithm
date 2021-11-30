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
    Application app;
    app.Run();
}