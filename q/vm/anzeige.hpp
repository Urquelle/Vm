#pragma once

#include <cstdint>

#include "laufwerk.hpp"

namespace Vm {

class Anzeige : public Laufwerk
{
public:
    Anzeige();

    Ergebnis<uint8_t> lesen_1byte(uint16_t adresse);
    bool schreiben_1byte(uint16_t adresse, uint8_t wert);
    bool schreiben_2byte(uint16_t adresse, uint16_t wert);
    void ausgeben(uint16_t adresse, uint16_t anzahl_bytes);

private:
    void gehe_zu(uint16_t x, uint16_t y);
    void anzeige_blank();
    void schrift_fett();
    void schrift_normal();
};

}
