#include "position.hpp"

Position::Position()
    : _index(0)
    , _q()
{
}

Position::Position(uint16_t index, std::string q)
    : _index(index)
    , _q(q)
{
}

uint16_t
Position::index() const
{
    return _index;
}

std::string
Position::q() const
{
    return _q;
}

inline int16_t operator-(const Position& lhs, const Position& rhs)
{
    int16_t erg = lhs.index() - rhs.index();

    return erg;
}
