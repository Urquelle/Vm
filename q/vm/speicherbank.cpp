#include "speicherbank.hpp"

namespace Vm {

Speicherbank::Speicherbank(Cpu *cpu, size_t z_laufwerke, size_t gr_laufwerk)
    : _cpu(cpu)
{
    for (uint32_t laufwerk_idx = 0; laufwerk_idx < z_laufwerke; ++laufwerk_idx)
    {
        _laufwerke.push_back(new Arbeitsspeicher(gr_laufwerk));
    }
}

Ergebnis<uint8_t>
Speicherbank::lesen_1byte(uint16_t adresse)
{
    auto laufwerk = laufwerk_holen();
    auto erg = laufwerk->lesen_1byte(adresse);

    return erg;
}

bool
Speicherbank::schreiben_1byte(uint16_t adresse, uint8_t wert)
{
    auto laufwerk = laufwerk_holen();
    auto erg = laufwerk->schreiben_1byte(adresse, wert);

    return true;
}

void
Speicherbank::ausgeben(uint16_t adresse, uint16_t anzahl_bytes)
{
    auto laufwerk = laufwerk_holen();
    laufwerk->ausgeben(adresse, anzahl_bytes);
}

Laufwerk *
Speicherbank::laufwerk_holen()
{
    auto bank_id = _cpu->regs[REG_MB];
    auto laufwerk = _laufwerke[bank_id];

    return laufwerk;
}


}
