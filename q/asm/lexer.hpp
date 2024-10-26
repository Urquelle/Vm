#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "allgemein/position.hpp"
#include "allgemein/zeichen.hpp"
#include "asm/token.hpp"

namespace Asm {

class Lexer
{
public:
    Lexer(std::string q, std::string text);

    std::vector<Token *> starten();

private:
    Zeichen zeichen();
    Zeichen weiter(uint16_t z = 1);
    void leerzeichen_einlesen();

    std::string _q;
    std::string _text;
    uint16_t _index;
};

}
