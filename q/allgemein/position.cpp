#include "position.hpp"

Position::Position()
    : _index(0)
    , _q()
{
}

Position::Position(uint64_t index, std::string q)
    : _index(index)
    , _q(q)
{
}

uint64_t
Position::index() const
{
    return _index;
}

std::string
Position::q() const
{
    return _q;
}

inline uint64_t operator-(const Position& lhs, const Position& rhs)
{
    uint64_t erg = lhs.index() - rhs.index();

    return erg;
}
