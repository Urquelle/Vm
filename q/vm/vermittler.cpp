#include "vermittler.hpp"

namespace Vm {

Vermittler::Vermittler()
{
}

Ergebnis<Vermittler::Region>
Vermittler::finden(uint16_t adresse)
{
    for (auto region : _regionen)
    {
        if ((adresse >= region.anfang) && (region.anfang + region.größe >= adresse))
        {
            return region;
        }
    }

    return new Fehler("keine passende region gefunden");
}

Ergebnis<uint16_t>
Vermittler::lesen_2byte(uint16_t adresse)
{
    auto region = finden(adresse);

    if (!region.gut())
    {
        return region.fehler();
    }

    uint16_t adr = region.wert().zuordnen
        ? adresse - region.wert().anfang
        : adresse;

    return region.wert().laufwerk->lesen_2byte(adr);
}

Ergebnis<uint8_t>
Vermittler::lesen_1byte(uint16_t adresse)
{
    auto region = finden(adresse);

    if (!region.gut())
    {
        return region.fehler();
    }

    uint16_t adr = region.wert().zuordnen
        ? adresse - region.wert().anfang
        : adresse;

    return region.wert().laufwerk->lesen_1byte(adr);
}

bool
Vermittler::schreiben_2byte(uint16_t adresse, uint16_t wert)
{
    auto region = finden(adresse);

    if (!region.gut())
    {
        return false;
    }

    uint16_t adr = region.wert().zuordnen
        ? adresse - region.wert().anfang
        : adresse;

    return region.wert().laufwerk->schreiben_2byte(adr, wert);
}

bool
Vermittler::schreiben_1byte(uint16_t adresse, uint8_t wert)
{
    auto region = finden(adresse);

    if (!region.gut())
    {
        return false;
    }

    uint16_t adr = region.wert().zuordnen
        ? adresse - region.wert().anfang
        : adresse;

    return region.wert().laufwerk->schreiben_1byte(adr, wert);
}

void
Vermittler::ausgeben(uint16_t adresse, uint16_t anzahl_bytes)
{
    auto region = finden(adresse);

    if (!region.gut())
    {
        return;
    }

    uint16_t adr = region.wert().zuordnen
        ? adresse - region.wert().anfang
        : adresse;

    region.wert().laufwerk->ausgeben(adr, anzahl_bytes);
}

}
