#pragma once

#include <cstdint>

#include "vm/laufwerk.hpp"

namespace Vm {

class Anweisung;
class Cpu;

#define Flags_Liste \
    X(FL_UNBEKANNT, 0) \
    X(FL_POS, 1) \
    X(FL_ZRO, 2) \
    X(FL_NEG, 4)
enum Flags
{
    #define X(Name, Wert) Name = Wert,
    Flags_Liste
    #undef X

    Z_FLAGS,
};

#define Register_Liste \
    X(REG_R1,    1)  \
    X(REG_R2,    2)  \
    X(REG_R3,    3)  \
    X(REG_R4,    4)  \
    X(REG_R5,    5)  \
    X(REG_R6,    6)  \
    X(REG_R7,    7)  \
    X(REG_R8,    8)  \
    X(REG_IP,    9)  \
    X(REG_SP,    10) \
    X(REG_FP,    11) \
    X(REG_MB,    12) \
    X(REG_ACU,   13) \
    X(REG_FLAGS, 14) \
    X(REG_IM,    15)
enum Register : std::int8_t
{
    #define X(Name, Wert, ...) Name = Wert,
    Register_Liste
    #undef X

    Z_REGISTER,
};

class Cpu {
public:
    Cpu(Laufwerk *laufwerk, uint32_t interrupt_vektor_adresse = 0x1000);

    bool     push(uint16_t wert);
    uint16_t pop();

    void     stand_speichern();
    void     stand_laden();

    void     schritt();
    void     ausführen(uint16_t adresse = 0);

    uint8_t  lesen();
    uint16_t lesen2();

    void     ausgeben();

    uint16_t regs[Register::Z_REGISTER];

    Laufwerk *laufwerk;
    bool halt;
    bool pause;
    uint16_t z_stack_rahmen;

    uint32_t interrupt_vektor_adresse;
    bool     in_interrupt_steuerung;
};

}

