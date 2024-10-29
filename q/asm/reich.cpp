#include "reich.hpp"

namespace Asm {

Reich::Reich()
    : _name()
    , _über(nullptr)
{
}

Reich::Reich(std::string name, Reich *über)
    : _name(name)
    , _über(über)
{
}

std::string
Reich::name()
{
    return _name;
}

Reich *
Reich::über()
{
    return _über;
}

bool
Reich::registriert(std::string name)
{
    auto erg = _symbole.contains(name);

    return erg;
}

bool
Reich::registrieren(std::string name, Symbol *symbol)
{
    if (registriert(name))
    {
        return false;
    }

    _symbole[name] = symbol;

    return true;
}

Symbol *
Reich::symbol(std::string name)
{
    auto *erg = _symbole[name];

    return erg;
}

Ergebnis<Symbol *>
Reich::suchen(std::string name)
{
    Reich *reich = this;
    Symbol *sym = nullptr;

    while (reich)
    {
        sym = reich->symbol(name);
        if (sym != nullptr)
        {
            return Ergebnis(sym);
        }

        reich = reich->über();
    }

    return Ergebnis<Symbol *>(Fehler("symbol wurde nicht gefunden."));
}

}

