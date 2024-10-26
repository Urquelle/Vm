#pragma once

#include <cstdint>
#include <vector>

#include "allgemein/ergebnis.hpp"
#include "vm/laufwerk.hpp"

namespace Vm {

class Arbeitsspeicher : public Laufwerk
{
public:
    Arbeitsspeicher(size_t z_speicher);
    Arbeitsspeicher(size_t z_speicher, uint16_t adresse, std::vector<uint16_t> programm);

    Ergebnis<uint8_t> lesen_1byte(uint16_t adresse);
    bool schreiben_1byte(uint16_t adresse, uint8_t wert);
    void ausgeben(uint16_t adresse, uint16_t anzahl_bytes);

private:
    uint8_t *_daten;
};

}
