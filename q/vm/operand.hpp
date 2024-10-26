#pragma once

#include <cstdint>

namespace Vm {

class Operand
{
public:
    enum Art
    {
        OPND_REG = 0,
        OPND_LIT = 1,
        OPND_ADR = 2,
        OPND_ZGR = 3,
    };

    uint8_t art() { return _art; }
    uint16_t größe() { return _größe; }
    template<typename T> T als();

    bool gleich(uint32_t art) { return _art == art; }

    static Operand * Reg(uint8_t   wert);
    static Operand * Lit(uint16_t  wert);
    static Operand * Adr(uint16_t  wert);
    static Operand * Zgr(Operand * wert);

protected:
    Operand(uint8_t art, uint16_t z);

private:
    uint8_t _art;
    uint16_t _größe;
};

class Operand_Reg : public Operand
{
public:
    Operand_Reg(uint8_t wert);

    uint8_t wert() { return _wert; }

private:
    uint8_t _wert;
};

class Operand_Lit : public Operand
{
public:
    Operand_Lit(uint16_t wert);

    uint16_t wert() { return _wert; }

private:
    uint16_t _wert;
};

class Operand_Adr : public Operand
{
public:
    Operand_Adr(uint16_t wert);

    uint16_t wert() { return _wert; }
    void setzen(uint16_t wert) { _wert = wert; }

private:
    uint16_t _wert;
};

class Operand_Zgr : public Operand
{
public:
    Operand_Zgr(Operand *wert);

    Operand * wert() { return _wert; }

private:
    Operand *_wert;
};

}
