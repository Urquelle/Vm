#include "ergebnis.hpp"

template <typename TWert>
Ergebnis<TWert>::Ergebnis(TWert wert)
    : _wert(wert), _gut(true)
{
}

template <typename TWert>
Ergebnis<TWert>::Ergebnis(Fehler fehler)
    : _fehler(fehler), _gut(false)
{
}

template <typename TWert>
TWert Ergebnis<TWert>::wert()
{
    return _wert;
}

template <typename TWert>
Fehler Ergebnis<TWert>::fehler()
{
    return _fehler;
}

template <typename TWert>
bool Ergebnis<TWert>::gut()
{
    return _gut;
}

template <typename TWert>
bool Ergebnis<TWert>::schlecht()
{
    return !_gut;
}
