#include "anzeige.hpp"

#include <cmath>
#include <cstdio>

namespace Vm {

Anzeige::Anzeige()
{
    _vol = 0;
}

Ergebnis<uint8_t>
Anzeige::lesen_1byte(uint16_t adresse)
{
    return 0;
}

bool
Anzeige::schreiben_1byte(uint16_t adresse, uint8_t wert)
{
    return false;
}

bool
Anzeige::schreiben_2byte(uint16_t adresse, uint16_t wert)
{
    auto anweisung = (wert & 0xff00) >> 8;

    if (anweisung == 0xff)
    {
        anzeige_blank();
    }
    else if (anweisung == 0x01)
    {
        schrift_fett();
    }
    else if (anweisung == 0x02)
    {
        schrift_normal();
    }

    auto zeichen   = (wert & 0x00ff) >> 0;

    uint16_t x   = (uint16_t) ((adresse % 16) + 1);
    uint16_t y   = (uint16_t) (std::floor(adresse / 16) + 1);

    gehe_zu(x * 2, y);
    printf("%c", zeichen);

    return true;
}

void
Anzeige::ausgeben(uint16_t adresse, uint16_t anzahl_bytes)
{
    //
}

void
Anzeige::gehe_zu(uint16_t x, uint16_t y)
{
    printf("\x1b[%d;%dH", y, x);
}

void
Anzeige::anzeige_blank()
{
    printf("\x1b[2J");
}

void
Anzeige::schrift_fett()
{
    printf("\x1b[1m");
}

void
Anzeige::schrift_normal()
{
    printf("\x1b[0m");
}

}
