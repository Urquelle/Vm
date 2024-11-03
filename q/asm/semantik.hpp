#pragma once

#include <vector>
#include <map>

#include "allgemein/ergebnis.hpp"
#include "allgemein/fehler.hpp"
#include "asm/symbol.hpp"
#include "asm/zone.hpp"
#include "vm/operand.hpp"

namespace Asm {

class Semantik
{
public:
    Semantik(Ast ast);

    Ast starten();

    void makros_registrieren();
    void makros_erweitern();
    void markierungen_registrieren();

    bool symbol_registrieren(std::string name, Symbol *symbol);
    bool symbol_registriert(std::string name);
    Symbol *symbol_holen(std::string name);

    void        anweisungen_analysieren();
    Anweisung * anweisung_analysieren(Asm::Anweisung *anweisung);
    Anweisung * anweisung_kopieren(Asm::Anweisung *anweisung);

    Anweisung * mov_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * add_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * dec_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * inc_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * jne_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * hlt_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * rti_analysieren(Asm::Anweisung_Asm *anweisung);
    Anweisung * brk_analysieren(Asm::Anweisung_Asm *anweisung);

    Vm::Operand * operand_analysieren(Ausdruck *op);

    uint16_t ausdruck_auswerten(Ausdruck *ausdruck);

    Diagnostik diagnostik();
    void melden(Spanne spanne, Fehler *fehler);
    void melden(Anweisung *anweisung, Fehler *fehler);
    void melden(Ausdruck *ausdruck, Fehler *fehler);
    void melden(Token *token, Fehler *fehler);

    Zone *zone();
    Zone *zone_betreten(Zone *zone);
    void zone_verlassen();

private:
    Zone *_zone;
    Ast _ast;
    Diagnostik _diagnostik;
};

}
