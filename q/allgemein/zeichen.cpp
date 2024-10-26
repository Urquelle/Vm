#include "allgemein/zeichen.hpp"

Zeichen::Zeichen(char c, uint16_t index, std::string q, char *text)
    : _c(c)
    , _text(text)
    , _position(Position(index, q))
{
}


Zeichen::Zeichen(Zeichen &z)
    : _c(z.c())
    , _text(z.text())
    , _position(z.pos())
{
}

Position
Zeichen::pos()
{
    return _position;
}

char
Zeichen::c() const
{
    return _c;
}

char *
Zeichen::text()
{
    return _text;
}

inline bool operator==(const Zeichen& lhs, const char rhs)
{
    return lhs.c() == rhs;
}

inline bool operator!=(const Zeichen& lhs, const char rhs)
{
    return !(lhs == rhs);
}

