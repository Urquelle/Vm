#pragma once

#include <cstdint>
#include <string>

class Position
{
public:
    Position();
    Position(uint64_t index, std::string q);

    uint64_t index() const;
    std::string q() const;

private:

    uint64_t _index;
    std::string _q;
};

inline uint64_t operator-(const Position& lhs, const Position& rhs);
