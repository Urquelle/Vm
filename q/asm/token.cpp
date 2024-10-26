#include "asm/token.hpp"

namespace Asm {

Token::Token(uint8_t art, Zeichen z, Position anfang, uint16_t laenge)
    : _art(art)
    , _position(anfang)
    , _laenge(laenge)
{
    _text = new char[laenge + 1];
    memcpy(_text, z.text() + _position.index(), _laenge);
    _text[_laenge] = 0;
}

uint8_t
Token::art()
{
    return _art;
}

Position
Token::pos()
{
    return _position;
}

uint16_t
Token::laenge()
{
    return _laenge;
}

char *
Token::text()
{
    return _text;
}

void
Token::ausgeben()
{
    printf("%s", Token_Namen(_art));
}

template<typename T>
T Token::als()
{
    return static_cast<T> (this);
}

Token *
Token::Eof(Zeichen z)
{
    auto erg = new Token(Token::T_EOF, z, z.pos(), 1);

    return erg;
}

Token *
Token::Text(Zeichen anfang, Zeichen ende)
{
    auto erg = new Token(Token::T_TEXT, anfang, anfang.pos(), ende.pos() - anfang.pos());

    return erg;
}

Token *
Token::Name(Zeichen anfang, Zeichen ende)
{
    auto erg = new Token(Token::T_NAME, anfang, anfang.pos(), ende.pos() - anfang.pos());

    return erg;
}

Token *
Token::Ganzzahl(Zeichen anfang, Zeichen ende, uint32_t zahl, uint16_t basis)
{
    auto erg = new Token_Ganzzahl(anfang, ende, zahl, basis);

    return erg;
}

Token *
Token::Hex(Zeichen anfang, Zeichen ende, uint16_t zahl)
{
    auto erg = new Token_Hex(anfang, ende, zahl);

    return erg;
}

Token *
Token::Runde_Klammer_Auf(Zeichen z)
{
    auto erg = new Token(Token::T_RUNDE_KLAMMER_AUF, z, z.pos(), 1);

    return erg;
}

Token *
Token::Runde_Klammer_Zu(Zeichen z)
{
    auto erg = new Token(Token::T_RUNDE_KLAMMER_ZU, z, z.pos(), 1);

    return erg;
}

Token *
Token::Eckige_Klammer_Auf(Zeichen z)
{
    auto erg = new Token(Token::T_ECKIGE_KLAMMER_AUF, z, z.pos(), 1);

    return erg;
}

Token *
Token::Eckige_Klammer_Zu(Zeichen z)
{
    auto erg = new Token(Token::T_ECKIGE_KLAMMER_ZU, z, z.pos(), 1);

    return erg;
}

Token *
Token::Geschweifte_Klammer_Auf(Zeichen z)
{
    auto erg = new Token(Token::T_GESCHWEIFTE_KLAMMER_AUF, z, z.pos(), 1);

    return erg;
}

Token *
Token::Geschweifte_Klammer_Zu(Zeichen z)
{
    auto erg = new Token(Token::T_GESCHWEIFTE_KLAMMER_ZU, z, z.pos(), 1);

    return erg;
}

Token *
Token::Groesser(Zeichen z)
{
    auto erg = new Token(Token::T_GROESSER, z, z.pos(), 1);

    return erg;
}

Token *
Token::Groesser_Gleich(Zeichen z)
{
    auto erg = new Token(Token::T_GROESSER_GLEICH, z, z.pos(), 2);

    return erg;
}

Token *
Token::Kleiner_Gleich(Zeichen z)
{
    auto erg = new Token(Token::T_KLEINER_GLEICH, z, z.pos(), 2);

    return erg;
}

Token *
Token::Kleiner(Zeichen z)
{
    auto erg = new Token(Token::T_KLEINER, z, z.pos(), 1);

    return erg;
}

Token *
Token::Gleich(Zeichen z)
{
    auto erg = new Token(Token::T_GLEICH, z, z.pos(), 1);

    return erg;
}

Token *
Token::Gleich_Gleich(Zeichen z)
{
    auto erg = new Token(Token::T_GLEICH_GLEICH, z, z.pos(), 2);

    return erg;
}

Token *
Token::Punkt(Zeichen z)
{
    auto erg = new Token(Token::T_PUNKT, z, z.pos(), 1);

    return erg;
}

Token *
Token::Komma(Zeichen z)
{
    auto erg = new Token(Token::T_KOMMA, z, z.pos(), 1);

    return erg;
}

Token *
Token::Plus(Zeichen z)
{
    auto erg = new Token(Token::T_PLUS, z, z.pos(), 1);

    return erg;
}

Token *
Token::Minus(Zeichen z)
{
    auto erg = new Token(Token::T_MINUS, z, z.pos(), 1);

    return erg;
}

Token *
Token::Stern(Zeichen z)
{
    auto erg = new Token(Token::T_STERN, z, z.pos(), 1);

    return erg;
}

Token *
Token::Pisa(Zeichen z)
{
    auto erg = new Token(Token::T_PISA, z, z.pos(), 1);

    return erg;
}

Token *
Token::Pisa_Links(Zeichen z)
{
    auto erg = new Token(Token::T_PISA_LINKS, z, z.pos(), 1);

    return erg;
}

Token *
Token::Raute(Zeichen z)
{
    auto erg = new Token(Token::T_RAUTE, z, z.pos(), 1);

    return erg;
}

Token *
Token::Prozent(Zeichen z)
{
    auto erg = new Token(Token::T_PROZENT, z, z.pos(), 1);

    return erg;
}

Token *
Token::Klammeraffe(Zeichen z)
{
    auto erg = new Token(Token::T_KLAMMERAFFE, z, z.pos(), 1);

    return erg;
}

Token *
Token::Doppelpunkt(Zeichen z)
{
    auto erg = new Token(Token::T_DOPPELPUNKT, z, z.pos(), 1);

    return erg;
}

Token *
Token::Semikolon(Zeichen z)
{
    auto erg = new Token(Token::T_SEMIKOLON, z, z.pos(), 1);

    return erg;
}

Token *
Token::Ausrufezeichen(Zeichen z)
{
    auto erg = new Token(Token::T_AUSRUFEZEICHEN, z, z.pos(), 1);

    return erg;
}

Token *
Token::Kaufmannsund(Zeichen z)
{
    auto erg = new Token(Token::T_KAUFMANNSUND, z, z.pos(), 1);

    return erg;
}

Token *
Token::Balken(Zeichen z)
{
    auto erg = new Token(Token::T_BALKEN, z, z.pos(), 1);

    return erg;
}

Token *
Token::Zeilenumbruch(Zeichen z)
{
    auto erg = new Token(Token::T_ZEILENUMBRUCH, z, z.pos(), 1);

    return erg;
}

Token_Ganzzahl::Token_Ganzzahl(Zeichen anfang, Zeichen ende, uint32_t zahl, uint16_t basis)
    : Token(T_GANZZAHL, anfang, anfang.pos(), ende.pos() - anfang.pos())
    , _zahl(zahl)
    , _basis(basis)
{
}

uint32_t
Token_Ganzzahl::zahl()
{
    return _zahl;
}

void
Token_Ganzzahl::ausgeben()
{
    if (_basis == 10)
    {
        printf("%s(%d)", Token_Namen(_art), _zahl);
    }
    else if (_basis == 16)
    {
        printf("%s(0x%04X)", Token_Namen(_art), _zahl);
    }
    else if (_basis == 2)
    {
        printf("%s(%d)", Token_Namen(_art), _zahl);
    }
    else
    {
        printf("%s(%d)", Token_Namen(_art), _zahl);
    }
}

Token_Hex::Token_Hex(Zeichen anfang, Zeichen ende, uint16_t zahl)
    : Token(T_HEX, anfang, anfang.pos(), ende.pos() - anfang.pos())
    , _zahl(zahl)
{
}

uint16_t
Token_Hex::zahl()
{
    return _zahl;
}

void
Token_Hex::ausgeben()
{
    printf("%s($%04X)", Token_Namen(_art), _zahl);
}

char *
Token_Namen(uint16_t token_id)
{
    switch (token_id)
    {
    #define X(Name) case Token::Name: return (char *) #Name;
    Token_Liste
    #undef X
    }

    return (char *) "unbekannt";
}

}

