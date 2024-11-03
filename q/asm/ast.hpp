#pragma once

#include <cstdint>
#include <list>
#include <string.h>

#include "vm/anweisung.hpp"

namespace Asm {

class Anweisung;
class Ausdruck;
class Deklaration;
class Hex;
class Name;

struct Ast
{
    std::list<Deklaration *> deklarationen;
    std::list<Anweisung *> anweisungen;
};

// deklarationen {{{
#define Deklaration_Art \
    X(KONSTANTE, 1, "Konstante") \
    X(DATEN, 2, "Daten") \
    X(SCHABLONE, 3, "Schablone") \
    X(MAKRO, 4, "Makro")

class Deklaration
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Deklaration_Art
        #undef X
    };

    Deklaration(Deklaration::Art art, Spanne spanne, std::string name, bool exportieren = false);

    virtual void ausgeben(uint8_t tiefe, std::ostream &ausgabe) = 0;
    template<typename T> T als();

    Art art() const;
    Spanne spanne() const;
    std::string name();
    bool exportieren();

private:
    bool _exportieren;
    Art _art;
    Spanne _spanne;
    std::string _name;
};

class Deklaration_Konstante : public Deklaration
{
public:
    Deklaration_Konstante(Spanne spanne, std::string name, uint16_t wert, bool exportieren);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;

    uint16_t wert() { return _wert; }

private:
    uint16_t _wert;
};

class Deklaration_Daten : public Deklaration
{
public:
    Deklaration_Daten(Spanne spanne, uint16_t größe, std::string name, uint16_t anzahl, std::vector<Hex *> daten, bool exportieren);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;

    std::vector<Hex *> daten();
    uint16_t anzahl();
    uint16_t größe();
    uint32_t gesamtgröße();

    uint16_t adresse;

private:
    uint16_t _anzahl;
    uint16_t _größe;
    std::vector<Hex *> _daten;
};

class Deklaration_Schablone : public Deklaration
{
public:
    class Feld
    {
    public:
        Feld(Spanne spanne, std::string name, uint16_t größe)
        : _spanne(spanne), _name(name), _größe(größe) {}

        Spanne spanne() const { return _spanne; }
        std::string name() { return _name; }
        uint16_t größe() { return _größe; }

    private:
        Spanne _spanne;
        std::string _name;
        uint16_t _größe;
    };

    Deklaration_Schablone(Spanne spanne, std::string name, std::vector<Feld *> felder);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    std::vector<Feld *> felder();

private:
    std::vector<Feld *> _felder;
};

class Deklaration_Makro : public Deklaration
{
public:
    Deklaration_Makro(Spanne spanne, std::string name, std::vector<Name *> parameter, std::vector<Anweisung *> rumpf);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    std::vector<Name *> parameter();
    std::vector<Anweisung *> rumpf();

private:
    std::vector<Name *> _parameter;
    std::vector<Anweisung *> _rumpf;
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
    X(ADRESSE, 10, "Adresse") \
    X(BLOCK, 11, "Block")

class Ausdruck
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Ausdruck_Art
        #undef X
    };

    Ausdruck(Art art, Spanne spanne)
        : _art(art)
        , _spanne(spanne)
    {
    }

    virtual void ausgeben(uint8_t tiefe, std::ostream &ausgabe) = 0;
    virtual Ausdruck *kopie() = 0;
    template<typename T> T als();

    Art art() const;
    Spanne spanne() const;
    bool ungleich(Art art);
    bool gleich(Art art);

private:
    Art _art;
    Spanne _spanne;
};

class Bin : public Ausdruck
{
public:
    Bin(Spanne spanne, Token *op, Ausdruck *links, Ausdruck *rechts);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Token *op() { return _op; }
    Ausdruck *links()  { return _links;  }
    Ausdruck *rechts() { return _rechts; }

private:
    Token *_op;
    Ausdruck *_links;
    Ausdruck *_rechts;
};

class Name : public Ausdruck
{
public:
    Name(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    std::string name();

private:
    std::string _name;
};

class Auswertung : public Ausdruck
{
public:
    Auswertung(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck * ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Reg : public Ausdruck
{
public:
    Reg(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    uint32_t reg();
    std::string name();

private:
    std::string _name;
};

class Adresse : public Ausdruck
{
public:
    Adresse(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;
    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Hex : public Ausdruck
{
public:
    Hex(Spanne spanne, uint16_t wert);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    uint16_t wert();

private:
    uint16_t _wert;
};

class Text : public Ausdruck
{
public:
    Text(Spanne spanne, std::string text);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

private:
    std::string _text;
};

class Variable : public Ausdruck
{
public:
    Variable(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;
    std::string name();

private:
    std::string _name;
};

class Klammer : public Ausdruck
{
public:
    Klammer(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Als : public Ausdruck
{
public:
    Als(Spanne spanne, std::string schablone, std::string basis, std::string feld);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

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
#define Anweisung_Art \
    X(ASM,        1, "Asm") \
    X(MAKRO,      2, "Makro") \
    X(MARKIERUNG, 3, "Markierung")

class Anweisung
{
public:
    enum Art
    {
    #define X(N, W, ...) N = W,
        Anweisung_Art
    #undef X
    };

    Anweisung(Art art, Spanne spanne);

    virtual void ausgeben(uint8_t tiefe, std::ostream &ausgabe) = 0;
    virtual Anweisung *kopie() = 0;

    template<typename T> T als();

    Vm::Anweisung *vm_anweisung();
    void vm_anweisung_setzen(Vm::Anweisung *vm_anweisung);

    void adresse_setzen(uint16_t adresse);
    uint16_t adresse();

    Art art() const;
    Spanne spanne() const;

private:
    Art _art;
    Spanne _spanne;
    uint16_t _adresse;
    Vm::Anweisung *_vm_anweisung;
};

class Anweisung_Asm : public Anweisung
{
public:
    Anweisung_Asm(Spanne spanne, std::string op, std::vector<Ausdruck *> operanden);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Anweisung *kopie() override;

    std::string op();
    std::vector<Ausdruck *> operanden();

    uint32_t größe();

private:
    std::string _op;
    std::vector<Ausdruck *> _operanden;
};

class Anweisung_Makro : public Anweisung
{
public:
    Anweisung_Makro(Spanne spanne, std::string name, std::vector<Ausdruck *> operanden);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Anweisung *kopie() override;

    std::string name() const;
    std::vector<Ausdruck *> argumente();

private:
    std::string _name;
    std::vector<Ausdruck *> _argumente;
};

class Anweisung_Markierung : public Anweisung
{
public:
    Anweisung_Markierung(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Anweisung *kopie() override;

    std::string name();

private:
    std::string _name;
};
// }}}

}
