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
    enum Art
    {
        Deklaration = 1,
        Anweisung = 2,
    };

    struct Zeile
    {
        Art art;
        void *daten;
    };

    Syntax(std::vector<Token *> token);

    Ast starten();

    Asm::Anweisung   * anweisung_einlesen();
    Asm::Deklaration * daten_dekl_einlesen(uint32_t z_daten, bool exportieren);
    Asm::Deklaration * schablone_dekl_einlesen(bool exportieren);

    Asm::Ausdruck * ausdruck_einlesen();
    Asm::Ausdruck * operand_einlesen();
    Asm::Ausdruck * plus_ausdruck_einlesen();
    Asm::Ausdruck * mult_ausdruck_einlesen();
    Asm::Ausdruck * basis_ausdruck_einlesen();

    template<typename T>
    T       brauche(Asm::Ausdruck::Art art);
    Token * token();
    Token * weiter();
    Token * erwarte(Token::Art art);
    bool    akzeptiere(Token::Art art);
    bool    gleich(Token::Art art);
    bool    ungleich(Token::Art art);

    Zeile   zeile_einlesen();
    Zeile   zeile_deklaration(Asm::Deklaration *dekl);
    Zeile   zeile_anweisung(Asm::Anweisung *anw);

private:
    std::vector<Token *> _token;
    uint32_t _token_index;
};

}
