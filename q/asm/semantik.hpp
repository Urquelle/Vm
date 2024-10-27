#pragma once

#include <vector>
#include <map>

#include "vm/operand.hpp"

namespace Asm {

class Semantik
{
public:
    Semantik(std::vector<Ast_Knoten *> anweisungen);

    std::vector<Ast_Knoten *> starten();
    void markierungen_registrieren();
    void anweisung_analysieren(Ast_Knoten *anweisung);

    void mov_analysieren(Ast_Anweisung *mov);
    void add_analysieren(Ast_Anweisung *mov);
    void dec_analysieren(Ast_Anweisung *mov);
    void inc_analysieren(Ast_Anweisung *mov);
    void jne_analysieren(Ast_Anweisung *mov);
    void hlt_analysieren(Ast_Anweisung *mov);

    void markierung_analysieren(Ast_Knoten *name, uint16_t wert);
    Vm::Operand * operand_analysieren(Ast_Knoten *op);
    uint16_t ausdruck_auswerten(Ast_Knoten *ausdruck);

    const std::map<std::string, uint16_t> markierungen() { return _markierungen; }

private:
    std::map<std::string, uint16_t> _markierungen;
    std::vector<Ast_Knoten *> _anweisungen;

    uint16_t _adresse;
    bool _in_code;
};

}
