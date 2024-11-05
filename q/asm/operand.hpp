#pragma once

#include "asm/symbol.hpp"

namespace Asm {

class Operand
{
public:
    Operand(Symbol *symbol);

    Symbol *symbol();

private:
    Symbol *_symbol;
};

}
