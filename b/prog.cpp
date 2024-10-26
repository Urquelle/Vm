#include "allgemein/fehler.cpp"
#include "allgemein/ergebnis.cpp"
#include "vm/laufwerk.cpp"
#include "vm/arbeitsspeicher.cpp"
#include "vm/vermittler.cpp"
#include "vm/operand.cpp"
#include "vm/anweisung.cpp"
#include "vm/cpu.cpp"

int main (int argc, char **argv)
{
    using namespace Vm;

    auto vermittler = new Vermittler();
    auto speicher = new Arbeitsspeicher(256*256);

    vermittler->zuordnen({
        .laufwerk = speicher,
        .anfang = 0x0,
        .ende = 0xffff
    });

    auto cpu = new Cpu(vermittler);
    uint16_t adr = 0;

    #define VMOP(Opr) adr; adr += (Opr).wert()

    #if 0
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_ADR, {Operand::Reg(REG_R1), Operand::Adr(0x100)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R2), Operand::Lit(0x1)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_ADD_REG_REG, {Operand::Reg(REG_R1), Operand::Reg(REG_R2)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_ADR_REG, {Operand::Adr(0x0100), Operand::Reg(REG_ACC)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_JNE,         {Operand::Lit(3), Operand::Adr(0x0)}, adr));
    #elif 0
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R1), Operand::Lit(0x5151)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R2), Operand::Lit(0x4242)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_REG,     {Operand::Reg(REG_R1)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_REG,     {Operand::Reg(REG_R2)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_POP,         {Operand::Reg(REG_R1)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_POP,         {Operand::Reg(REG_R2)}, adr));
    #else
    uint16_t adr_subroutine = 0x3000;

    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x3333)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x2222)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x1111)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R1), Operand::Lit(0x1234)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R4), Operand::Lit(0x5678)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x0)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_CAL_ADR,     {Operand::Adr(adr_subroutine)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x4444)}, adr));

    adr = adr_subroutine;

    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x0102)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x0304)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_PSH_LIT,     {Operand::Lit(0x0506)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R1), Operand::Lit(0x0708)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_MOV_REG_LIT, {Operand::Reg(REG_R8), Operand::Lit(0x090A)}, adr));
    VMOP(Anweisung::Kodieren(speicher, OP_RET,         {}, adr));
    #endif

    cpu->ausführen();

    // cpu->speicher->ausgeben(0x0100, 8);
    // cpu->ausgeben();

    return 0;
}
