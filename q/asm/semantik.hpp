#pragma once

#include <vector>
#include <map>

#include "asm/symbol.hpp"
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
    void mov_analysieren(Asm::Anweisung *anweisung);
    void add_analysieren(Asm::Anweisung *anweisung);
    void dec_analysieren(Asm::Anweisung *anweisung);
    void inc_analysieren(Asm::Anweisung *anweisung);
    void jne_analysieren(Asm::Anweisung *anweisung);
    void hlt_analysieren(Asm::Anweisung *anweisung);

    Vm::Operand * operand_analysieren(Ausdruck *op);
    uint16_t ausdruck_auswerten(Ausdruck *ausdruck);

private:
    std::map<std::string, Symbol *> _symbole;
    Ast _ast;
};

}
