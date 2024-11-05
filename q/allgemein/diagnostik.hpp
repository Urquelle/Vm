#pragma once

#include <vector>

#include "allgemein/fehler.hpp"
#include "allgemein/spanne.hpp"

#define FEHLER_WENN(A, K, F) do { if (A) { melden((K), (F)); } } while(0)

class Diagnostik
{
public:
    struct Meldung
    {
        Spanne spanne;
        Fehler * fehler;
    };

    Diagnostik();

    bool hat_meldungen();
    void melden(Meldung meldung);
    void melden(Spanne spanne, Fehler *fehler);
    std::vector<Meldung> meldungen();

private:
    std::vector<Meldung> _meldungen;
};
