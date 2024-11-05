#include "asm/operand.hpp"

namespace Asm {

Operand::Operand(Symbol *symbol)
    : _symbol(symbol)
{
}

Symbol *
Operand::symbol()
{
    return _symbol;
}

}
