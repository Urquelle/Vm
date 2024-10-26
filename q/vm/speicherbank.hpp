#pragma once

#include <cstdint>
#include <vector>

#include "allgemein/ergebnis.hpp"
#include "vm/laufwerk.hpp"
#include "vm/cpu.hpp"

namespace Vm {

class Speicherbank : public Laufwerk
{
public:
    Speicherbank(Cpu *cpu, size_t z_laufwerke, size_t gr_laufwerk);

    Ergebnis<uint8_t> lesen_1byte(uint16_t adresse);
    bool schreiben_1byte(uint16_t adresse, uint8_t wert);
    void ausgeben(uint16_t adresse, uint16_t anzahl_bytes);

protected:
    Laufwerk * laufwerk_holen();

private:
    Cpu *_cpu;
    std::vector<Laufwerk *> _laufwerke;
};

}

