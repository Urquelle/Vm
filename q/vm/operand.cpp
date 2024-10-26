#include "operand.hpp"

namespace Vm {

Operand::Operand(uint8_t art, uint16_t größe)
    : _art(art)
    , _größe(größe)
{
}

Operand *
Operand::Reg(uint8_t reg)
{
    auto erg = new Operand_Reg(reg);

    return erg;
}

Operand *
Operand::Lit(uint16_t lit)
{
    auto erg = new Operand_Lit(lit);

    return erg;
}

Operand *
Operand::Adr(uint16_t adr)
{
    auto erg = new Operand_Adr(adr);

    return erg;
}

Operand *
Operand::Zgr(Operand *opr)
{
    auto erg = new Operand_Zgr(opr);

    return erg;
}

Operand_Reg::Operand_Reg(uint8_t wert)
    : Operand(Operand::OPND_REG, 1)
    , _wert(wert)
{
}

Operand_Lit::Operand_Lit(uint16_t wert)
    : Operand(Operand::OPND_LIT, 2)
    , _wert(wert)
{
}

Operand_Adr::Operand_Adr(uint16_t wert)
    : Operand(Operand::OPND_ADR, 2)
    , _wert(wert)
{
}

Operand_Zgr::Operand_Zgr(Operand *wert)
    : Operand(Operand::OPND_ZGR, 1)
    , _wert(wert)
{
}

template<typename T>
T Operand::als()
{
    return static_cast<T> (this);
}

}
