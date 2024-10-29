#include "diagnostik.hpp"

#include <iostream>

Diagnostik::Diagnostik()
{
}

bool
Diagnostik::hat_meldungen()
{
    auto erg = _meldungen.size() > 0;

    return erg;
}

void
Diagnostik::melden(Diagnostik::Meldung meldung)
{
    _meldungen.push_back(meldung);
}

void
Diagnostik::melden(Position position, Fehler *fehler)
{
    _meldungen.push_back({
        .position = position,
        .fehler = fehler
    });
}

std::vector<Diagnostik::Meldung>
Diagnostik::meldungen()
{
    return _meldungen;
}

std::ostream& operator<<(std::ostream& ausgabe, const Diagnostik::Meldung& m)
{
    ausgabe << "index: " << m.position.index() << " in " << m.position.q() << "Fehler: " << m.fehler->text << std::endl;

    return ausgabe;
}
