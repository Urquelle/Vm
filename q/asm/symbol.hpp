#pragma once

#include <cstdint>
#include <string>

#include "asm/ast.hpp"

namespace Asm {

class Zone;

class Symbol
{
public:
    enum Art
    {
        KONSTANTE   = 1,
        DATEN       = 2,
        SCHABLONE   = 3,
        ANWEISUNG   = 4,
        MAKRO       = 5,
        PLATZHALTER = 6,
        MARKIERUNG  = 7,
        MODUL       = 8,
        FELD        = 9,
    };

    Symbol(Art art, std::string name);

    template<typename T> T als();

    Art art();
    std::string name();

    void zone_setzen(Zone *zone);
    Zone *zone();

private:
    Art _art;
    std::string _name;
    Zone *_zone;
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

    Symbol_Schablone(std::string name, Zone *zone);
};

class Symbol_Makro : public Symbol
{
public:
    Symbol_Makro(std::string name, Deklaration_Makro *dekl);

    std::vector<Ausdruck_Name *> parameter();
    Deklaration_Makro *dekl();

private:
    Deklaration_Makro *_dekl;
};

class Symbol_Platzhalter : public Symbol
{
public:
    Symbol_Platzhalter(std::string name);

    void ausdruck_setzen(Ausdruck *ausdruck);
    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Symbol_Markierung : public Symbol
{
public:
    Symbol_Markierung(std::string name, uint16_t adresse);

    uint16_t adresse();
private:
    uint16_t _adresse;
};

class Symbol_Modul : public Symbol
{
public:
    Symbol_Modul(std::string name, Modul *modul);

    Modul *modul() const;

private:
    Modul *_modul;
};

class Symbol_Feld : public Symbol
{
public:
    Symbol_Feld(std::string name, uint16_t versatz, uint16_t größe);

    uint16_t versatz();
    uint16_t größe();

private:
    uint16_t _versatz;
    uint16_t _größe;
};

}
