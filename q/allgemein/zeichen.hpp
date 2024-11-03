#pragma once

#include <cstdint>
#include <string>

#include "allgemein/position.hpp"

class Zeichen
{
public:
    Zeichen();
    Zeichen(const Zeichen& z);
    Zeichen(char c, uint16_t index, std::string& q, std::string& text);

    Position pos() const;
    std::string text() const;
    char * zgr() const;
    char c() const;

private:
    char _c;
    std::string _text;
    Position _position;
};

inline bool operator==(const Zeichen& lhs, const char rhs);
inline bool operator!=(const Zeichen& lhs, const char rhs);

