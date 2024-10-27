#pragma once

#include <vector>
#include <map>

#include "asm/symbol.hpp"
#include "vm/operand.hpp"

namespace Asm {

class Semantik
{
public:
    Semantik(std::vector<Ast_Knoten *> anweisungen);

    std::vector<Ast_Knoten *> starten();

    void markierungen_registrieren();
    bool symbol_registrieren(std::string name, Symbol *symbol);
    Symbol *symbol_holen(std::string name);

    void anweisung_analysieren(Ast_Knoten *anweisung);
    void mov_analysieren(Ast_Anweisung *mov);
    void add_analysieren(Ast_Anweisung *mov);
    void dec_analysieren(Ast_Anweisung *mov);
    void inc_analysieren(Ast_Anweisung *mov);
    void jne_analysieren(Ast_Anweisung *mov);
    void hlt_analysieren(Ast_Anweisung *mov);

    Vm::Operand * operand_analysieren(Ast_Knoten *op);
    uint16_t ausdruck_auswerten(Ast_Knoten *ausdruck);

private:
    std::map<std::string, Symbol *> _symbole;
    std::vector<Ast_Knoten *> _anweisungen;
};

}
