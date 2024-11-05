#include "cpu.hpp"
#include "anweisung.hpp"

#include <cassert>
#include <iostream>
#include <format>

namespace Vm {

Cpu::Cpu(Laufwerk *laufwerk, uint32_t interrupt_vektor_adresse)
    : laufwerk(laufwerk)
    , halt(false)
    , pause(false)
    , interrupt_vektor_adresse(interrupt_vektor_adresse)
{
    z_stack_rahmen = 0;

    for (int i = REG_R1; i < Z_REGISTER; ++i)
    {
        regs[i] = 0;
    }

    regs[REG_FLAGS] = FL_ZRO;
    regs[REG_SP]    = 0xffff - 1;
    regs[REG_FP]    = regs[REG_SP];
    regs[REG_IM]    = 0xffff;
}

bool
Cpu::push(uint16_t wert)
{
    if (laufwerk->schreiben_2byte(regs[REG_SP], wert))
    {
        regs[REG_SP] -= 2;
        z_stack_rahmen += 2;

        return true;
    }

    return false;
}

void
Cpu::stand_speichern()
{
    for (uint8_t reg_index = REG_R1; reg_index <= REG_R8; ++reg_index)
    {
        push(regs[reg_index]);
    }

    push(regs[REG_IP]);
    push(z_stack_rahmen + 2);

    regs[REG_FP] = regs[REG_SP];
    z_stack_rahmen = 0;
}

void
Cpu::stand_laden()
{
    auto fp = regs[REG_FP];
    regs[REG_SP] = fp;

    z_stack_rahmen = pop();
    auto lokal_z_stack_rahmen = z_stack_rahmen;

    regs[REG_IP] = pop();

    for (uint8_t reg_index = REG_R8; reg_index >= REG_R1; --reg_index)
    {
        regs[reg_index] = pop();
    }

    auto z_args = pop();

    for (uint8_t args_index = 0; args_index < z_args; ++args_index)
    {
        pop();
    }

    regs[REG_FP] = fp + lokal_z_stack_rahmen;
}

uint16_t
Cpu::pop()
{
    regs[REG_SP] += 2;
    auto erg = laufwerk->lesen_2byte(regs[REG_SP]);
    z_stack_rahmen -= 2;

    return erg.wert();
}

void
Cpu::schritt()
{
    uint8_t daten = lesen();
    auto anweisung = Anweisung::Dekodieren(this, daten);

    if (anweisung)
    {
        assert(anweisung != nullptr);
        anweisung->starten(this);
    }
}

void
Cpu::ausführen(uint16_t adresse)
{
    regs[REG_IP] = adresse;
    while (!halt)
    {
        schritt();

        if (pause)
        {
            __debugbreak();
            pause = false;
        }
    }
}

uint8_t
Cpu::lesen()
{
    auto ip = regs[REG_IP];
    uint8_t erg = laufwerk->lesen_1byte(ip).wert();
    regs[REG_IP] += 1;

    return erg;
}

uint16_t
Cpu::lesen2()
{
    auto ip = regs[REG_IP];
    auto erg = laufwerk->lesen_2byte(ip).wert();
    regs[REG_IP] += 2;

    return erg;
}

void
Cpu::ausgeben()
{
    auto &ausgabe = std::cout;

    ausgabe << "+------------ CPU ---------------+";
    ausgabe << std::endl;
    ausgabe << std::format(" | R1: {:#04X}", regs[REG_R1]);
    ausgabe << std::format(" | R2: {:#04X}", regs[REG_R2]);
    ausgabe << " |" << std::endl;
    ausgabe << std::format(" | R3: {:#04X}", regs[REG_R3]);
    ausgabe << std::format(" | R4: {:#04X}", regs[REG_R4]);
    ausgabe << std::format(" | R5: {:#04X}", regs[REG_R5]);
    ausgabe << " |" << std::endl;
    ausgabe << std::format(" | R6: {:#04X}", regs[REG_R6]);
    ausgabe << std::format(" | R7: {:#04X}", regs[REG_R7]);
    ausgabe << std::endl;
    ausgabe << "+--------------------------------+" << std::endl;
}

}
