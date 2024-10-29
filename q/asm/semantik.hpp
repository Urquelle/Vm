#pragma once

#include <vector>
#include <map>

#include "allgemein/ergebnis.hpp"
#include "allgemein/fehler.hpp"
#include "asm/symbol.hpp"
#include "asm/reich.hpp"
#include "vm/operand.hpp"

namespace Asm {

class Semantik
{
public:
    Semantik(Ast ast);

    Ast starten();

    void markierungen_registrieren();
    bool symbol_registrieren(std::string name, Symbol *symbol);
    Symbol *symbol_holen(std::string name);

    void anweisung_analysieren(Asm::Anweisung *anweisung);
    void mov_analysieren(Asm::Anweisung_Asm *anweisung);
    void add_analysieren(Asm::Anweisung_Asm *anweisung);
    void dec_analysieren(Asm::Anweisung_Asm *anweisung);
    void inc_analysieren(Asm::Anweisung_Asm *anweisung);
    void jne_analysieren(Asm::Anweisung_Asm *anweisung);
    void hlt_analysieren(Asm::Anweisung_Asm *anweisung);
    void rti_analysieren(Asm::Anweisung_Asm *anweisung);
    void brk_analysieren(Asm::Anweisung_Asm *anweisung);

    Vm::Operand * operand_analysieren(Ausdruck *op);
    uint16_t ausdruck_auswerten(Ausdruck *ausdruck);

    Diagnostik diagnostik();
    void melden(Position pos, Fehler *fehler);

private:
    Reich _reich;
    Ast _ast;
    Diagnostik _diagnostik;
};

}
