#pragma once

#include <cstdint>
#include <vector>

#include "allgemein/ergebnis.hpp"
#include "vm/laufwerk.hpp"

namespace Vm {

class Vermittler : public Laufwerk
{
public:
    struct Region
    {
        Laufwerk *laufwerk;
        uint16_t anfang;
        uint16_t größe;
        bool     zuordnen;
    };

    Vermittler();

    auto zuordnen(Region region)
    {
        auto erg = _regionen.insert(_regionen.begin(), region);

        return erg;
    }

    Ergebnis<Region>   finden(uint16_t adresse);
    Ergebnis<uint16_t> lesen_2byte(uint16_t adresse);
    Ergebnis<uint8_t>  lesen_1byte(uint16_t adresse);
    bool               schreiben_2byte(uint16_t adresse, uint16_t wert);
    bool               schreiben_1byte(uint16_t adresse, uint8_t wert);
    void               ausgeben(uint16_t adresse, uint16_t anzahl_bytes);

private:
    std::vector<Region> _regionen;
};

}
