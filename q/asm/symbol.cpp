#include "asm/symbol.hpp"
#include "symbol.hpp"

namespace Asm {

Symbol::Symbol(Art art, std::string name)
    : _art(art)
    , _name(name)
    , _zone(nullptr)
{
}

void
Symbol::zone_setzen(Zone *zone)
{
    _zone = zone;
}

Zone *
Symbol::zone()
{
    return _zone;
}

Symbol::Art
Symbol::art()
{
    return _art;
}

std::string
Symbol::name()
{
    return _name;
}

template<typename T>
T Symbol::als()
{
    return static_cast<T> (this);
}

Symbol_Konstante::Symbol_Konstante(std::string name, uint16_t wert)
    : Symbol(Symbol::KONSTANTE, name)
    , _wert(wert)
{
}

uint16_t
Symbol_Konstante::wert()
{
    return _wert;
}

Symbol_Daten::Symbol_Daten(std::string name, uint16_t adresse)
    : Symbol(Symbol::DATEN, name)
    , _adresse(adresse)
{
}

uint16_t
Symbol_Daten::adresse()
{
    return _adresse;
}

Symbol_Anweisung::Symbol_Anweisung(std::string name, uint16_t adresse)
    : Symbol(Symbol::ANWEISUNG, name)
    , _adresse(adresse)
{
}

uint16_t
Symbol_Anweisung::adresse()
{
    return _adresse;
}

Symbol_Schablone::Symbol_Schablone(std::string name, std::map<std::string, Symbol_Schablone::Feld *> felder)
    : Symbol(Symbol::SCHABLONE, name)
    , _felder(felder)
{
}

std::map<std::string, Symbol_Schablone::Feld *>
Symbol_Schablone::felder()
{
    return _felder;
}

Symbol_Makro::Symbol_Makro(std::string name, Deklaration_Makro *dekl)
    : Symbol(Symbol::MAKRO, name)
    , _dekl(dekl)
{
}

Deklaration_Makro *
Symbol_Makro::dekl()
{
    return _dekl;
}

std::vector<Name *>
Symbol_Makro::parameter()
{
    return _dekl->parameter();
}

Symbol_Platzhalter::Symbol_Platzhalter(std::string name)
    : Symbol(Symbol::PLATZHALTER, name)
{
}

void
Symbol_Platzhalter::ausdruck_setzen(Ausdruck *ausdruck)
{
    _ausdruck = ausdruck;
}

Ausdruck *
Symbol_Platzhalter::ausdruck()
{
    return _ausdruck;
}

Symbol_Markierung::Symbol_Markierung(std::string name, uint16_t adresse)
    : Symbol(Symbol::MARKIERUNG, name)
{
}

uint16_t
Symbol_Markierung::adresse()
{
    return _adresse;
}

}
