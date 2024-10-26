#pragma once

#include <cstdint>
#include <string>

#include "allgemein/position.hpp"

class Zeichen
{
public:
    Zeichen(char c, uint16_t index, std::string q, char *text);
    Zeichen(Zeichen &z);

    Position pos();
    char * text();
    char c() const;

private:
    char _c;
    char *_text;
    Position _position;
};

inline bool operator==(const Zeichen& lhs, const char rhs);
inline bool operator!=(const Zeichen& lhs, const char rhs);

