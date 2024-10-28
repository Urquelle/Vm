#pragma once

#include <cstdint>
#include <string.h>

#include "vm/anweisung.hpp"

namespace Asm {

class Anweisung;
class Ausdruck;
class Ast_Hex;
class Deklaration;

struct Ast
{
    std::vector<Deklaration *> deklarationen;
    std::vector<Anweisung *> anweisungen;
};

// deklarationen {{{
#define Deklaration_Art \
    X(KONSTANTE, 1, "Konstante") \
    X(DATEN, 2, "Daten") \
    X(SCHABLONE, 3, "Schablone")

class Deklaration
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Deklaration_Art
        #undef X
    };

    Deklaration(Deklaration::Art art, std::string name, bool exportieren = false);

    virtual void ausgeben(uint8_t tiefe) = 0;
    template<typename T> T als();

    Art art();
    std::string name();
    bool exportieren();

private:
    bool _exportieren;
    Art _art;
    std::string _name;
};

class Ast_Konstante : public Deklaration
{
public:
    Ast_Konstante(std::string name, uint16_t wert, bool exportieren);

    void ausgeben(uint8_t tiefe) override;

    uint16_t wert() { return _wert; }

private:
    uint16_t _wert;
};

class Ast_Daten : public Deklaration
{
public:
    Ast_Daten(uint16_t größe, std::string name, uint16_t anzahl, std::vector<Ast_Hex *> daten, bool exportieren);

    void ausgeben(uint8_t tiefe) override;

    std::vector<Ast_Hex *> daten();
    uint16_t anzahl();
    uint16_t größe();
    uint32_t gesamtgröße();

    uint16_t adresse;

private:
    uint16_t _anzahl;
    uint16_t _größe;
    std::vector<Ast_Hex *> _daten;
};

class Ast_Schablone : public Deklaration
{
public:
    class Feld
    {
    public:
        Feld(std::string name, uint16_t größe) : _name(name), _größe(größe) {}

        std::string name() { return _name; }
        uint16_t größe() { return _größe; }

    private:
        std::string _name;
        uint16_t _größe;
    };

    Ast_Schablone(std::string name, std::vector<Feld *> felder);

    void ausgeben(uint8_t tiefe) override;
    std::vector<Feld *> felder();

private:
    std::vector<Feld *> _felder;
};
// }}}
// ausdruck {{{
#define Ausdruck_Art \
    X(NAME, 1, "Name") \
    X(VARIABLE, 2, "Variable") \
    X(REG, 3, "Reg") \
    X(HEX, 4, "Hex") \
    X(BIN, 5, "Bin") \
    X(KLAMMER, 6, "Klammer") \
    X(AUSWERTUNG, 7, "Auswertung") \
    X(ALS, 8, "Als") \
    X(TEXT, 9, "Text") \
    X(ADRESSE, 10, "Adresse")

class Ausdruck
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Ausdruck_Art
        #undef X
    };

    Ausdruck(Art art)
        : _art(art)
    {
    }

    virtual void ausgeben(uint8_t tiefe) = 0;
    template<typename T> T als();

    Art art();
    bool ungleich(Art art);
    bool gleich(Art art);

private:
    Art _art;
};

class Ast_Bin : public Ausdruck
{
public:
    Ast_Bin(Token *op, Ausdruck *links, Ausdruck *rechts);

    void ausgeben(uint8_t tiefe) override;

    Token *op() { return _op; }
    Ausdruck *links()  { return _links;  }
    Ausdruck *rechts() { return _rechts; }

private:
    Token *_op;
    Ausdruck *_links;
    Ausdruck *_rechts;
};

class Ast_Name : public Ausdruck
{
public:
    Ast_Name(std::string name);

    void ausgeben(uint8_t tiefe) override;

    std::string name();

private:
    std::string _name;
};

class Ast_Auswertung : public Ausdruck
{
public:
    Ast_Auswertung(Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe) override;

    Ausdruck * ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ast_Reg : public Ausdruck
{
public:
    Ast_Reg(std::string name);

    void ausgeben(uint8_t tiefe) override;

    uint32_t reg();
    std::string name();

private:
    std::string _name;
};

class Ast_Adresse : public Ausdruck
{
public:
    Ast_Adresse(Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe) override;
    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ast_Hex : public Ausdruck
{
public:
    Ast_Hex(uint16_t wert);

    void ausgeben(uint8_t tiefe) override;

    uint16_t wert();

private:
    uint16_t _wert;
};

class Ast_Text : public Ausdruck
{
public:
    Ast_Text(std::string text);

    void ausgeben(uint8_t tiefe) override;

private:
    std::string _text;
};

class Ast_Variable : public Ausdruck
{
public:
    Ast_Variable(std::string name);

    void ausgeben(uint8_t tiefe) override;
    std::string name();

private:
    std::string _name;
};

class Ast_Klammer : public Ausdruck
{
public:
    Ast_Klammer(Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe) override;

    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ast_Als : public Ausdruck
{
public:
    Ast_Als(std::string schablone, std::string basis, std::string feld);
    void ausgeben(uint8_t tiefe) override;

    std::string schablone();
    std::string basis();
    std::string feld();

private:
    std::string _schablone;
    std::string _basis;
    std::string _feld;
};
// }}}
// anweisung {{{
class Anweisung
{
public:
    Anweisung(Ast_Name *markierung, std::string op, std::vector<Ausdruck *> operanden);

    void ausgeben(uint8_t tiefe);

    std::string op();
    std::vector<Ausdruck *> operanden();
    Ast_Name *markierung();

    uint32_t größe();
    Vm::Anweisung *anweisung();
    void anweisung_setzen(Vm::Anweisung *anweisung);

    uint16_t adresse;

private:
    Ast_Name *_markierung;
    std::string _op;
    std::vector<Ausdruck *> _operanden;
    Vm::Anweisung *_anweisung;
};
// }}}

}
