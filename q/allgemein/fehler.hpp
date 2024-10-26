#pragma once

#include <string>

struct Fehler
{
    Fehler();
    Fehler(std::string text);

    std::string text;
    uint32_t kennung;
};
