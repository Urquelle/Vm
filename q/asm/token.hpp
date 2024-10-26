#pragma once

#include <cstdint>
#include <ostream>

#include "allgemein/zeichen.hpp"
#include "allgemein/position.hpp"

#define Token_Liste \
    X(T_EOF) \
    X(T_ZEILENUMBRUCH) \
    X(T_RUNDE_KLAMMER_AUF) \
    X(T_RUNDE_KLAMMER_ZU) \
    X(T_ECKIGE_KLAMMER_AUF) \
    X(T_ECKIGE_KLAMMER_ZU) \
    X(T_GESCHWEIFTE_KLAMMER_AUF) \
    X(T_GESCHWEIFTE_KLAMMER_ZU) \
    X(T_GROESSER) \
    X(T_GROESSER_GLEICH) \
    X(T_KLEINER) \
    X(T_KLEINER_GLEICH) \
    X(T_GLEICH) \
    X(T_GLEICH_GLEICH) \
    X(T_NAME) \
    X(T_GANZZAHL) \
    X(T_HEX) \
    X(T_TEXT) \
    X(T_KLAMMERAFFE) \
    X(T_PUNKT) \
    X(T_KOMMA) \
    X(T_DOPPELPUNKT) \
    X(T_SEMIKOLON) \
    X(T_PLUS) \
    X(T_MINUS) \
    X(T_STERN) \
    X(T_PISA) \
    X(T_PISA_LINKS) \
    X(T_RAUTE) \
    X(T_KAUFMANNSUND) \
    X(T_BALKEN) \
    X(T_AUSRUFEZEICHEN) \
    X(T_PROZENT)

namespace Asm {

class Token
{
public:
    enum
    {
        #define X(Name) Name,
        Token_Liste
        #undef X

        Z_TOKENS
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

    uint8_t art();
    Position pos();
    uint16_t laenge();
    char * text();

    virtual void ausgeben();

    template<typename T> T als();

protected:
    Token(uint8_t art, Zeichen z, Position anfang, uint16_t laenge);

    uint8_t _art;
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

