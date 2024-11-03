#pragma once

#include "zeichen.hpp"

class Spanne
{
public:
    Spanne();
    Spanne(const Spanne& s);
    Spanne(const Zeichen& von, const Zeichen& bis);

    Zeichen von() const;
    Zeichen bis() const;
    uint32_t länge() const;
    char * anfang() const;

private:
    Zeichen _von;
    Zeichen _bis;
};
