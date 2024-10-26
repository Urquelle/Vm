#pragma once

#include "fehler.hpp"

template <typename TWert>
class Ergebnis
{
public:
    Ergebnis(TWert wert);
    Ergebnis(Fehler fehler);

    TWert wert();
    Fehler fehler();

    bool gut();
    bool schlecht();

private:
    bool _gut;

    TWert _wert;
    Fehler _fehler;
};
