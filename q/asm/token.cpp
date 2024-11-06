#include "asm/token.hpp"

#include <format>

namespace Asm {


Token::Token(Art art, Spanne spanne)
    : _art(art)
    , _spanne(spanne)
{
    _text = new char[_spanne.länge() + 1];

    char *anfang = _spanne.anfang();
    memcpy(_text, anfang, _spanne.länge());

    _text[_spanne.länge()] = 0;
}

Token::Art
Token::art() const
{
    return _art;
}

Position
Token::pos() const
{
    return _position;
}

Spanne
Token::spanne() const
{
    return _spanne;
}

uint16_t
Token::länge() const
{
    return _spanne.länge();
}

char *
Token::text()
{
    return _text;
}

void
Token::ausgeben(std::ostream &ausgabe)
{
    ausgabe << Token_Namen(_art);
}

template<typename T>
T Token::als()
{
    return static_cast<T> (this);
}

Token *
Token::Eof(Spanne spanne)
{
    auto erg = new Token(Token::ENDE, spanne);

    return erg;
}

Token *
Token::Text(Spanne spanne)
{
    auto erg = new Token(Token::TEXT, spanne);

    return erg;
}

Token *
Token::Name(Spanne spanne)
{
    auto erg = new Token(Token::NAME, spanne);

    return erg;
}

Token *
Token::Ganzzahl(Spanne spanne, uint32_t zahl, uint16_t basis)
{
    auto erg = new Token_Ganzzahl(spanne, zahl, basis);

    return erg;
}

Token *
Token::Hex(Spanne spanne, uint16_t wert)
{
    auto erg = new Token_Hex(spanne, wert);

    return erg;
}

Token *
Token::Runde_Klammer_Auf(Spanne spanne)
{
    auto erg = new Token(Token::RUNDE_KLAMMER_AUF, spanne);

    return erg;
}

Token *
Token::Runde_Klammer_Zu(Spanne spanne)
{
    auto erg = new Token(Token::RUNDE_KLAMMER_ZU, spanne);

    return erg;
}

Token *
Token::Eckige_Klammer_Auf(Spanne spanne)
{
    auto erg = new Token(Token::ECKIGE_KLAMMER_AUF, spanne);

    return erg;
}

Token *
Token::Eckige_Klammer_Zu(Spanne spanne)
{
    auto erg = new Token(Token::ECKIGE_KLAMMER_ZU, spanne);

    return erg;
}

Token *
Token::Geschweifte_Klammer_Auf(Spanne spanne)
{
    auto erg = new Token(Token::GESCHWEIFTE_KLAMMER_AUF, spanne);

    return erg;
}

Token *
Token::Geschweifte_Klammer_Zu(Spanne spanne)
{
    auto erg = new Token(Token::GESCHWEIFTE_KLAMMER_ZU, spanne);

    return erg;
}

Token *
Token::Groesser(Spanne spanne)
{
    auto erg = new Token(Token::GRÖSSER, spanne);

    return erg;
}

Token *
Token::Groesser_Gleich(Spanne spanne)
{
    auto erg = new Token(Token::GRÖSSER_GLEICH, spanne);

    return erg;
}

Token *
Token::Kleiner_Gleich(Spanne spanne)
{
    auto erg = new Token(Token::KLEINER_GLEICH, spanne);

    return erg;
}

Token *
Token::Kleiner(Spanne spanne)
{
    auto erg = new Token(Token::KLEINER, spanne);

    return erg;
}

Token *
Token::Gleich(Spanne spanne)
{
    auto erg = new Token(Token::GLEICH, spanne);

    return erg;
}

Token *
Token::Gleich_Gleich(Spanne spanne)
{
    auto erg = new Token(Token::GLEICH_GLEICH, spanne);

    return erg;
}

Token *
Token::Punkt(Spanne spanne)
{
    auto erg = new Token(Token::PUNKT, spanne);

    return erg;
}

Token *
Token::Komma(Spanne spanne)
{
    auto erg = new Token(Token::KOMMA, spanne);

    return erg;
}

Token *
Token::Plus(Spanne spanne)
{
    auto erg = new Token(Token::PLUS, spanne);

    return erg;
}

Token *
Token::Minus(Spanne spanne)
{
    auto erg = new Token(Token::MINUS, spanne);

    return erg;
}

Token *
Token::Stern(Spanne spanne)
{
    auto erg = new Token(Token::STERN, spanne);

    return erg;
}

Token *
Token::Pisa(Spanne spanne)
{
    auto erg = new Token(Token::PISA, spanne);

    return erg;
}

Token *
Token::Pisa_Links(Spanne spanne)
{
    auto erg = new Token(Token::PISA_LINKS, spanne);

    return erg;
}

Token *
Token::Raute(Spanne spanne)
{
    auto erg = new Token(Token::RAUTE, spanne);

    return erg;
}

Token *
Token::Prozent(Spanne spanne)
{
    auto erg = new Token(Token::PROZENT, spanne);

    return erg;
}

Token *
Token::Klammeraffe(Spanne spanne)
{
    auto erg = new Token(Token::KLAMMERAFFE, spanne);

    return erg;
}

Token *
Token::Doppelpunkt(Spanne spanne)
{
    auto erg = new Token(Token::DOPPELPUNKT, spanne);

    return erg;
}

Token *
Token::Semikolon(Spanne spanne)
{
    auto erg = new Token(Token::SEMIKOLON, spanne);

    return erg;
}

Token *
Token::Ausrufezeichen(Spanne spanne)
{
    auto erg = new Token(Token::AUSRUFEZEICHEN, spanne);

    return erg;
}

Token *
Token::Kaufmannsund(Spanne spanne)
{
    auto erg = new Token(Token::KAUFMANNSUND, spanne);

    return erg;
}

Token *
Token::Balken(Spanne spanne)
{
    auto erg = new Token(Token::BALKEN, spanne);

    return erg;
}

Token *
Token::Zeilenumbruch(Spanne spanne)
{
    auto erg = new Token(Token::ZEILENUMBRUCH, spanne);

    return erg;
}

Token_Ganzzahl::Token_Ganzzahl(Spanne spanne, uint32_t zahl, uint16_t basis)
    : Token(GANZZAHL, spanne)
    , _zahl(zahl)
    , _basis(basis)
{
}

uint32_t
Token_Ganzzahl::zahl()
{
    return _zahl;
}

uint16_t
Token_Ganzzahl::basis()
{
    return _basis;
}

void
Token_Ganzzahl::ausgeben(std::ostream &ausgabe)
{
    if (_basis == 10)
    {
        ausgabe << Token_Namen(_art) << std::format(" ({})", _zahl);
    }
    else if (_basis == 16)
    {
        ausgabe << Token_Namen(_art) << std::format("({:#06X})", _zahl);
    }
    else if (_basis == 2)
    {
        ausgabe << Token_Namen(_art) << std::format(" ({})", _zahl);
    }
    else
    {
        ausgabe << Token_Namen(_art) << std::format(" ({})", _zahl);
    }
}

Token_Hex::Token_Hex(Spanne spanne, uint16_t wert)
    : Token(HEX, spanne)
    , _wert(wert)
{
}

uint16_t
Token_Hex::wert()
{
    return _wert;
}

void
Token_Hex::ausgeben(std::ostream &ausgabe)
{
    ausgabe << Token_Namen(_art) << std::format("({:#06X})", _wert);
}

char *
Token_Namen(uint16_t token_id)
{
    switch (token_id)
    {
    #define X(N, W, B) case Token::N: return (char *) B;
    Token_Liste
    #undef X
    }

    return (char *) "unbekannt";
}

}

