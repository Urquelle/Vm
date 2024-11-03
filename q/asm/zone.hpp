#pragma once

#include <string>
#include <map>

namespace Asm {

class Symbol;

class Zone
{
public:
    Zone();
    Zone(std::string name, Zone *über = nullptr);

    Zone *über();
    void über_setzen(Zone *über);
    std::string name();
    bool registriert(std::string name);
    bool registrieren(std::string name, Symbol *symbol);
    Symbol * symbol(std::string name);
    Ergebnis<Symbol *> suchen(std::string name);

private:
    std::string _name;
    std::map<std::string, Symbol *> _symbole;
    Zone *_über;
};

}
