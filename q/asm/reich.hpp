#pragma once

#include <string>
#include <map>

#include "asm/symbol.hpp"

namespace Asm {

class Reich
{
public:
    Reich();
    Reich(std::string name, Reich *über = nullptr);

    Reich *über();
    std::string name();
    bool registriert(std::string name);
    bool registrieren(std::string name, Symbol *symbol);
    Symbol * symbol(std::string name);
    Ergebnis<Symbol *> suchen(std::string name);

private:
    std::string _name;
    std::map<std::string, Symbol *> _symbole;
    Reich *_über;
};

}
