#pragma once

#include <cstdint>
#include <ostream>

#include "allgemein/zeichen.hpp"
#include "allgemein/position.hpp"

#define Token_Liste \
    X(ENDE, 0, "Ende") \
    X(ZEILENUMBRUCH, 1, "Zeilenumbruch") \
    X(RUNDE_KLAMMER_AUF, 2, "(") \
    X(RUNDE_KLAMMER_ZU, 3, ")") \
    X(ECKIGE_KLAMMER_AUF, 4, "[") \
    X(ECKIGE_KLAMMER_ZU, 5, "]") \
    X(GESCHWEIFTE_KLAMMER_AUF, 6, "{") \
    X(GESCHWEIFTE_KLAMMER_ZU, 7, "}") \
    X(GROESSER, 8, ">") \
    X(GROESSER_GLEICH, 9, ">=") \
    X(KLEINER, 10, "<") \
    X(KLEINER_GLEICH, 11, "<=") \
    X(GLEICH, 12, "=") \
    X(GLEICH_GLEICH, 13, "==") \
    X(NAME, 14, "Name") \
    X(GANZZAHL, 15, "Ganzzahl") \
    X(HEX, 16, "Hex") \
    X(TEXT, 17, "Text") \
    X(KLAMMERAFFE, 18, "@") \
    X(PUNKT, 19, ".") \
    X(KOMMA, 20, ",") \
    X(DOPPELPUNKT, 21, ":") \
    X(SEMIKOLON, 22, ";") \
    X(PLUS, 23, "+") \
    X(MINUS, 24, "-") \
    X(STERN, 25, "*") \
    X(PISA, 26, "/") \
    X(PISA_LINKS, 27, "\\") \
    X(RAUTE, 28, "#") \
    X(KAUFMANNSUND, 29, "&") \
    X(BALKEN, 30, "|") \
    X(AUSRUFEZEICHEN, 31, "!") \
    X(PROZENT, 32, "%")

namespace Asm {

class Token
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Token_Liste
        #undef X
    };

    static Token * Eof(Zeichen z);
    static Token * Name(Zeichen anfang, Zeichen ende);
    static Token * Text(Zeichen anfang, Zeichen ende);
    static Token * Ganzzahl(Zeichen anfang, Zeichen ende, uint32_t zahl, uint16_t basis = 10);
    static Token * Hex(Zeichen anfang, Zeichen ende, uint16_t zahl);
    static Token * Runde_Klammer_Auf(Zeichen z);
    static Token * Runde_Klammer_Zu(Zeichen z);
    static Token * Eckige_Klammer_Auf(Zeichen z);
    static Token * Eckige_Klammer_Zu(Zeichen z);
    static Token * Geschweifte_Klammer_Auf(Zeichen z);
    static Token * Geschweifte_Klammer_Zu(Zeichen z);
    static Token * Gleich(Zeichen z);
    static Token * Groesser(Zeichen z);
    static Token * Groesser_Gleich(Zeichen z);
    static Token * Gleich_Gleich(Zeichen z);
    static Token * Kleiner_Gleich(Zeichen z);
    static Token * Kleiner(Zeichen z);
    static Token * Punkt(Zeichen z);
    static Token * Komma(Zeichen z);
    static Token * Plus(Zeichen z);
    static Token * Minus(Zeichen z);
    static Token * Stern(Zeichen z);
    static Token * Pisa(Zeichen z);
    static Token * Pisa_Links(Zeichen z);
    static Token * Raute(Zeichen z);
    static Token * Prozent(Zeichen z);
    static Token * Klammeraffe(Zeichen z);
    static Token * Doppelpunkt(Zeichen z);
    static Token * Semikolon(Zeichen z);
    static Token * Ausrufezeichen(Zeichen z);
    static Token * Kaufmannsund(Zeichen z);
    static Token * Balken(Zeichen z);
    static Token * Zeilenumbruch(Zeichen z);

    Art art();
    Position pos();
    uint16_t laenge();
    char * text();

    virtual void ausgeben();

    template<typename T> T als();

protected:
    Token(Art art, Zeichen z, Position anfang, uint16_t laenge);

    Art _art;
    Position _position;
    uint16_t _laenge;
    char *_text;
};

class Token_Ganzzahl : public Token
{
public:
    Token_Ganzzahl(Zeichen anfang, Zeichen ende, uint32_t zahl, uint16_t basis = 10);

    virtual void ausgeben();

    uint32_t zahl();
    uint16_t basis();

private:
    uint16_t _basis;
    uint32_t _zahl;
};

class Token_Hex : public Token
{
public:
    Token_Hex(Zeichen anfang, Zeichen ende, uint16_t zahl);

    virtual void ausgeben();

    uint16_t zahl();

private:
    uint16_t _zahl;
};

char * Token_Namen(uint16_t token_id);

}

