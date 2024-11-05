#pragma once

#include <string>

#include "allgemein/fehler.hpp"
#include "allgemein/diagnostik.hpp"
#include "asm/symbol.hpp"
#include "asm/zone.hpp"
#include "asm/operand.hpp"
#include "vm/operand.hpp"

namespace Asm {

class Semantik
{
public:
    Semantik(Ast ast, Zone *zone = new Zone("Welt", nullptr));

    Ast         starten(uint16_t start_adresse = 0);

    void        makros_registrieren();
    void        makros_erweitern();
    void        markierungen_registrieren();

    void        module_importieren();

    bool        symbol_registrieren(std::string name, Symbol *symbol);
    bool        symbol_registriert(std::string name);
    Symbol    * symbol_holen(std::string name);

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

    uint16_t       ausdruck_auswerten(Ausdruck *ausdruck);
    Ergebnis<Asm::Operand *, Fehler *> ausdruck_analysieren(Ausdruck *ausdruck);

    Vm::Operand * operand_analysieren(Ausdruck *op);

    Diagnostik diagnostik();
    void melden(Spanne spanne, Fehler *fehler);
    void melden(Anweisung *anweisung, Fehler *fehler);
    void melden(Ausdruck *ausdruck, Fehler *fehler);
    void melden(Token *token, Fehler *fehler);
    void melden(Modul *modul, Fehler *fehler);

    Zone * zone();
    Zone * zone_betreten(Zone *zone);
    void   zone_verlassen();

private:
    uint16_t _start_adresse;
    Zone *_zone;
    Ast _ast;
    Diagnostik _diagnostik;
};

}
