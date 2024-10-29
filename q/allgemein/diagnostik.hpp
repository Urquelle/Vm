#pragma once

#include <vector>

class Diagnostik
{
public:
    struct Meldung
    {
        Position position;
        Fehler * fehler;
    };

    Diagnostik();

    bool hat_meldungen();
    void melden(Meldung meldung);
    void melden(Position position, Fehler *fehler);
    std::vector<Meldung> meldungen();

private:
    std::vector<Meldung> _meldungen;
};
