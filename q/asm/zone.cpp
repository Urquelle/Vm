#include "zone.hpp"

#include <format>

namespace Asm {

Zone::Zone()
    : _name()
    , _über(nullptr)
{
}

Zone::Zone(std::string name, Zone *über)
    : _name(name)
    , _über(über)
{
}

std::string
Zone::name()
{
    return _name;
}

Zone *
Zone::über()
{
    return _über;
}

void
Zone::über_setzen(Zone *über)
{
    if (über == this)
    {
        return;
    }

    _über = über;
}

bool
Zone::registriert(std::string name)
{
    auto *erg = _symbole[name];

    return (erg != nullptr);
}

bool
Zone::registrieren(std::string name, Symbol *symbol)
{
    if (registriert(name))
    {
        return false;
    }

    _symbole[name] = symbol;

    return true;
}

Symbol *
Zone::symbol(std::string name)
{
    auto *erg = _symbole[name];

    return erg;
}

Ergebnis<Symbol *, Fehler *>
Zone::suchen(std::string name)
{
    Zone *zone = this;
    Symbol *sym = nullptr;

    while (zone)
    {
        sym = zone->symbol(name);
        if (sym != nullptr)
        {
            return Ergebnis<Symbol *, Fehler *>(sym);
        }

        zone = zone->über();
    }

    return Ergebnis<Symbol *, Fehler *>(new Fehler(std::format("symbol '{}' wurde nicht gefunden.", name)));
}

}
