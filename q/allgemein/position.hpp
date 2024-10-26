#pragma once

#include <cstdint>
#include <string>

class Position
{
public:
    Position(uint16_t index, std::string q);

    uint16_t index() const;
    std::string q() const;

private:

    uint16_t _index;
    std::string _q;
};

inline int16_t operator-(const Position& lhs, const Position& rhs);

