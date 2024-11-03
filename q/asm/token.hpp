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

    static Token * Eof(Spanne spanne);
    static Token * Name(Spanne spanne);
    static Token * Text(Spanne spanne);
    static Token * Ganzzahl(Spanne spanne, uint32_t zahl, uint16_t basis = 10);
    static Token * Hex(Spanne spanne, uint16_t wert);
    static Token * Runde_Klammer_Auf(Spanne spanne);
    static Token * Runde_Klammer_Zu(Spanne spanne);
    static Token * Eckige_Klammer_Auf(Spanne spanne);
    static Token * Eckige_Klammer_Zu(Spanne spanne);
    static Token * Geschweifte_Klammer_Auf(Spanne spanne);
    static Token * Geschweifte_Klammer_Zu(Spanne spanne);
    static Token * Gleich(Spanne spanne);
    static Token * Groesser(Spanne spanne);
    static Token * Groesser_Gleich(Spanne spanne);
    static Token * Gleich_Gleich(Spanne spanne);
    static Token * Kleiner_Gleich(Spanne spanne);
    static Token * Kleiner(Spanne spanne);
    static Token * Punkt(Spanne spanne);
    static Token * Komma(Spanne spanne);
    static Token * Plus(Spanne spanne);
    static Token * Minus(Spanne spanne);
    static Token * Stern(Spanne spanne);
    static Token * Pisa(Spanne spanne);
    static Token * Pisa_Links(Spanne spanne);
    static Token * Raute(Spanne spanne);
    static Token * Prozent(Spanne spanne);
    static Token * Klammeraffe(Spanne spanne);
    static Token * Doppelpunkt(Spanne spanne);
    static Token * Semikolon(Spanne spanne);
    static Token * Ausrufezeichen(Spanne spanne);
    static Token * Kaufmannsund(Spanne spanne);
    static Token * Balken(Spanne spanne);
    static Token * Zeilenumbruch(Spanne spanne);

    Art art() const;
    Spanne spanne() const;
    Position pos() const;
    uint16_t länge() const;
    char * text();

    virtual void ausgeben(std::ostream &ausgabe);

    template<typename T> T als();

protected:
    Token(Art art, Spanne spanne);

    Art _art;
    Spanne _spanne;
    Position _position;
    char *_text;
};

class Token_Ganzzahl : public Token
{
public:
    Token_Ganzzahl(Spanne spanne, uint32_t zahl, uint16_t basis = 10);

    virtual void ausgeben(std::ostream &ausgabe);

    uint32_t zahl();
    uint16_t basis();

private:
    uint16_t _basis;
    uint32_t _zahl;
};

class Token_Hex : public Token
{
public:
    Token_Hex(Spanne spanne, uint16_t wert);

    virtual void ausgeben(std::ostream &ausgabe);

    uint16_t wert();

private:
    uint16_t _wert;
};

char * Token_Namen(uint16_t token_id);

}

