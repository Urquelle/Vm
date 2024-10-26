#include "laufwerk.hpp"

namespace Vm {

Ergebnis<uint16_t>
Laufwerk::lesen_2byte(uint16_t adresse)
{
    auto unter = lesen_1byte(adresse + 1);

    if (!unter.gut())
    {
        return unter.fehler();
    }

    auto ober = lesen_1byte(adresse);

    if (!ober.gut())
    {
        return ober.fehler();
    }

    uint16_t erg = (ober.wert() << 8) | unter.wert();

    return Ergebnis<uint16_t>(erg);
}

bool
Laufwerk::schreiben_2byte(uint16_t adresse, uint16_t wert)
{
    // oberes byte
    if (!schreiben_1byte(adresse, wert >> 8))
    {
        return false;
    }

    // unteres byte
    if (!schreiben_1byte(adresse + 1, wert & 0xff))
    {
        return false;
    }

    return true;
}

size_t
Laufwerk::vol()
{
    return _vol;
}

}
