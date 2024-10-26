#pragma once

#include <cstdint>

#include "allgemein/ergebnis.hpp"

namespace Vm {

class Laufwerk
{
public:
    virtual Ergebnis<uint8_t> lesen_1byte(uint16_t adresse) = 0;
    virtual bool schreiben_1byte(uint16_t adresse, uint8_t wert) = 0;
    virtual void ausgeben(uint16_t adresse, uint16_t anzahl_bytes) = 0;

    virtual Ergebnis<uint16_t> lesen_2byte(uint16_t adresse);
    virtual bool schreiben_2byte(uint16_t adresse, uint16_t wert);
    virtual size_t vol();

protected:
    size_t _vol;
};

}
