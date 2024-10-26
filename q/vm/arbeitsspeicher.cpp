#include "arbeitsspeicher.hpp"

#include <iostream>

namespace Vm {

Arbeitsspeicher::Arbeitsspeicher(size_t vol)
{
    _vol = vol;
    _daten = (uint8_t *) calloc(_vol, sizeof(uint8_t));
}

Arbeitsspeicher::Arbeitsspeicher(size_t z_speicher, uint16_t adresse, std::vector<uint16_t> programm)
    : Arbeitsspeicher(z_speicher)
{
    if (_vol <= programm.size()*sizeof(uint16_t))
    {
        _vol = programm.size()*sizeof(uint16_t);
    }

    for (uint16_t index = 0; index < programm.size(); ++index)
    {
        auto wert = programm[index];
        schreiben_2byte(adresse + index*2, wert);
    }
}

Ergebnis<uint8_t>
Arbeitsspeicher::lesen_1byte(uint16_t adresse)
{
    if (adresse < 0 || adresse >= _vol)
    {
        return Ergebnis<uint8_t>(Fehler("adresse außerhalb des gültigen bereichs"));
    }

    auto erg = _daten[adresse];

    return Ergebnis<uint8_t>(erg);
}

bool
Arbeitsspeicher::schreiben_1byte(uint16_t adresse, uint8_t wert)
{
    if (adresse < 0 || adresse >= _vol)
    {
        return false;
    }

    _daten[adresse] = wert;

    return true;
}

void
Arbeitsspeicher::ausgeben(uint16_t adresse, uint16_t anzahl_bytes)
{
    if (adresse >= _vol)
    {
        std::cout << "Adresse ausserhalb des gültigen Bereichs." << std::endl;
    }

    int32_t i = 0;
    for (uint16_t index = adresse; index < adresse + anzahl_bytes; index += 2, i += 1)
    {
        if ((i % 4) == 0) printf("\n0x%04X: ", index);
        printf("%02X %02X ", _daten[index], _daten[index+1]);
    }
    printf("\n");
}

}

