#include "asm/symbol.hpp"

namespace Asm {

Symbol::Symbol(Art art, std::string name)
    : _art(art)
    , _name(name)
{
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
    : Symbol(Symbol::Konstante, name)
    , _wert(wert)
{
}

uint16_t
Symbol_Konstante::wert()
{
    return _wert;
}

Symbol_Daten::Symbol_Daten(std::string name, uint16_t adresse)
    : Symbol(Symbol::Daten, name)
    , _adresse(adresse)
{
}

uint16_t
Symbol_Daten::adresse()
{
    return _adresse;
}

Symbol_Anweisung::Symbol_Anweisung(std::string name, uint16_t adresse)
    : Symbol(Symbol::Anweisung, name)
    , _adresse(adresse)
{
}

uint16_t
Symbol_Anweisung::adresse()
{
    return _adresse;
}

Symbol_Schablone::Symbol_Schablone(std::string name, std::map<std::string, Symbol_Schablone::Feld *> felder)
    : Symbol(Symbol::Schablone, name)
    , _felder(felder)
{
}

std::map<std::string, Symbol_Schablone::Feld *>
Symbol_Schablone::felder()
{
    return _felder;
}

}
