#pragma once

#include <string>
#include <vector>

#include "allgemein/ergebnis.hpp"
#include "allgemein/diagnostik.hpp"
#include "asm/ast.hpp"
#include "asm/token.hpp"

namespace Asm {

class Syntax
{
public:
    enum Art
    {
        DEKLARATION = 1,
        ANWEISUNG   = 2,
    };

    struct Zeile
    {
        Art art;
        void *daten;
    };

    static Fehler *Fehler_Name_Erwartet;
    static Fehler *Fehler_Wert_Erwartet;
    static Fehler *Fehler_Token_Erwartet;

    Syntax(std::vector<Token *> token);

    Ast starten();

    Asm::Anweisung   * anweisung_einlesen();
    Asm::Anweisung   * import_anweisung_einlesen();
    Asm::Anweisung   * makro_anweisung_einlesen();
    Asm::Anweisung   * markierung_anweisung_einlesen();
    Asm::Anweisung   * asm_anweisung_einlesen();
    Asm::Deklaration * daten_dekl_einlesen(uint32_t z_daten, bool exportieren);
    Asm::Deklaration * schablone_dekl_einlesen(bool exportieren);
    Asm::Deklaration * makro_dekl_einlesen(bool exportieren);

    Asm::Ausdruck * ausdruck_einlesen();
    Asm::Ausdruck * operand_einlesen();
    Asm::Ausdruck * plus_ausdruck_einlesen();
    Asm::Ausdruck * mult_ausdruck_einlesen();
    Asm::Ausdruck * basis_ausdruck_einlesen();

    template<typename T>
    T       brauche(Asm::Ausdruck::Art art);
    Token * token(uint32_t versatz = 0);
    Token * weiter();
    Token * erwarte(Token::Art art);
    bool    akzeptiere(Token::Art art);
    bool    gleich(Token::Art art);
    bool    ungleich(Token::Art art);

    Zeile   zeile_einlesen();
    Zeile   zeile_deklaration(Asm::Deklaration *dekl);
    Zeile   zeile_anweisung(Asm::Anweisung *anw);

    Diagnostik diagnostik();
    void melden(Spanne spanne, Fehler *fehler);
    void melden(Token *token, Fehler *fehler);
    void melden(Ausdruck *ausdruck, Fehler *fehler);

private:
    std::vector<Token *> _token;
    uint32_t _token_index;
    Diagnostik _diagnostik;
};

}
