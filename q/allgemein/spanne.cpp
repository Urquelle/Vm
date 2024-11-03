#include "spanne.hpp"

#include <ostream>

Spanne::Spanne()
{
}

Spanne::Spanne(const Spanne& s)
    : _von(s.von())
    , _bis(s.bis())
{
}

Spanne::Spanne(const Zeichen& von, const Zeichen& bis)
    : _von(von)
    , _bis(bis)
{
}

Zeichen
Spanne::von() const
{
    return _von;
}

Zeichen
Spanne::bis() const
{
    return _bis;
}

uint32_t
Spanne::länge() const
{
    auto erg = _bis.pos().index() - _von.pos().index() + 1;

    return erg;
}

char *
Spanne::anfang() const
{
    char *erg = _von.zgr();

    return erg;
}

std::ostream& operator<<(std::ostream& ausgabe, const Spanne& spanne)
{
    ausgabe.write(spanne.anfang() + spanne.von().pos().index(), spanne.länge());

    return ausgabe;
}
