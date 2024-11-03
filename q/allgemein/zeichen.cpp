#include "allgemein/zeichen.hpp"

Zeichen::Zeichen()
{
}

Zeichen::Zeichen(const Zeichen &z)
    : _c(z.c())
    , _text(z.text())
    , _position(z.pos())
{
}

Zeichen::Zeichen(char c, uint16_t index, std::string& q, std::string& text)
    : _c(c)
    , _text(text)
    , _position(Position(index, q))
{
}

Position
Zeichen::pos() const
{
    return _position;
}

char
Zeichen::c() const
{
    return _c;
}

std::string
Zeichen::text() const
{
    return _text;
}

char *
Zeichen::zgr() const
{
    char *erg = (char *) _text.c_str() + _position.index();

    return erg;
}

inline bool operator==(const Zeichen& lhs, const char rhs)
{
    return lhs.c() == rhs;
}

inline bool operator!=(const Zeichen& lhs, const char rhs)
{
    return !(lhs == rhs);
}

