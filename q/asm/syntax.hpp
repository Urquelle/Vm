#pragma once

#include <string>
#include <vector>

#include "allgemein/ergebnis.hpp"
#include "asm/ast.hpp"
#include "asm/token.hpp"

namespace Asm {

class Syntax
{
public:
    Syntax(std::vector<Token *> token);

    std::vector<Ast_Knoten *> starten();

    Ast_Knoten * anweisung_einlesen();
    Ast_Knoten * daten_dekl_einlesen(uint32_t z_daten, bool exportieren);

    Ast_Knoten * zeile_analysieren();
    Ast_Knoten * ausdruck_einlesen();
    Ast_Knoten * operand_einlesen();
    Ast_Knoten * plus_ausdruck_einlesen();
    Ast_Knoten * mult_ausdruck_einlesen();
    Ast_Knoten * basis_ausdruck_einlesen();

    Token * token();
    Token * weiter();
    Token * erwarte(uint32_t art);
    bool    gleich(uint32_t art);
    bool    ungleich(uint32_t art);
    bool    akzeptiere(uint32_t art);

private:
    std::vector<Token *> _token;
    uint32_t _token_index;
};

}
