#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "vm/operand.hpp"

namespace Asm {

class Symbol
{
public:
    enum Art
    {
        Konstante = 1,
        Daten     = 2,
        Anweisung = 3,
        Schablone = 4,
    };

    Symbol(Art art, std::string name);

    template<typename T> T als();

    Art art();
    std::string name();

private:
    Art _art;
    std::string _name;
};

class Symbol_Konstante : public Symbol
{
public:
    Symbol_Konstante(std::string name, uint16_t wert);

    uint16_t wert();

private:
    uint16_t _wert;
};

class Symbol_Daten : public Symbol
{
public:
    Symbol_Daten(std::string name, uint16_t adresse);

    uint16_t adresse();

private:
    uint16_t _adresse;
};

class Symbol_Anweisung : public Symbol
{
public:
    Symbol_Anweisung(std::string name, uint16_t adresse);

    uint16_t adresse();

private:
    uint16_t _adresse;
};

class Symbol_Schablone : public Symbol
{
public:
    class Feld
    {
    public:
        Feld(uint16_t versatz, uint16_t größe)
            : _versatz(versatz), _größe(größe) {}

        uint16_t versatz() { return _versatz; }
        uint16_t größe() { return _größe; }

    private:
        uint16_t _versatz;
        uint16_t _größe;
    };

    Symbol_Schablone(std::string name, std::map<std::string, Feld *> felder);

    std::map<std::string, Feld *> felder();

private:
    std::map<std::string, Feld *> _felder;
};

}

