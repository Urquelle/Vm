#pragma once

#include <cstdint>
#include <list>

#include "allgemein/spanne.hpp"
#include "asm/token.hpp"
#include "vm/anweisung.hpp"

namespace Asm {

class Anweisung;
class Ausdruck;
class Ausdruck_Feld;
class Ausdruck_Hex;
class Ausdruck_Name;
class Deklaration;
class Modul;

struct Ast
{
    std::list<Deklaration *> deklarationen;
    std::list<Anweisung *> anweisungen;
    std::list<Modul *> module;
};

// modul {{{
class Modul
{
public:
    Modul(Spanne spanne, std::string name, uint16_t adresse);

    Spanne spanne() const;
    std::string name() const;
    uint16_t adresse() const;

    std::list<Deklaration *> deklarationen();
    void deklaration_hinzufügen(Deklaration *deklaration);

    std::list<Anweisung *> anweisungen();
    void anweisung_hinzufügen(Anweisung *anweisung);

private:
    Spanne _spanne;
    std::string _name;
    uint16_t _adresse;
    std::list<Deklaration *> _deklarationen;
    std::list<Anweisung *> _anweisungen;
};
// }}}
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
    Deklaration_Daten(Spanne spanne, uint16_t größe, std::string name, uint16_t anzahl,
                      std::vector<Ausdruck_Hex *> daten, bool exportieren);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;

    std::vector<Ausdruck_Hex *> daten();
    uint16_t anzahl();
    uint16_t größe();
    uint32_t gesamtgröße();

    uint16_t adresse;

private:
    uint16_t _anzahl;
    uint16_t _größe;
    std::vector<Ausdruck_Hex *> _daten;
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
    Deklaration_Makro(Spanne spanne, std::string name, std::vector<Ausdruck_Name *> parameter,
                      std::vector<Anweisung *> rumpf);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    std::vector<Ausdruck_Name *> parameter();
    std::vector<Anweisung *> rumpf();

private:
    std::vector<Ausdruck_Name *> _parameter;
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
    X(BLOCK, 11, "Block") \
    X(FELD, 12, "Feld") \
    X(GANZZAHL, 13, "Ganzzahl")

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

class Ausdruck_Bin : public Ausdruck
{
public:
    Ausdruck_Bin(Spanne spanne, Token *op, Ausdruck *links, Ausdruck *rechts);

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

class Ausdruck_Name : public Ausdruck
{
public:
    Ausdruck_Name(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    std::string name();

private:
    std::string _name;
};

class Ausdruck_Auswertung : public Ausdruck
{
public:
    Ausdruck_Auswertung(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck * ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ausdruck_Reg : public Ausdruck
{
public:
    Ausdruck_Reg(Spanne spanne, std::string name);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    uint32_t reg();
    std::string name();

private:
    std::string _name;
};

class Ausdruck_Adresse : public Ausdruck
{
public:
    Ausdruck_Adresse(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;
    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ausdruck_Hex : public Ausdruck
{
public:
    Ausdruck_Hex(Spanne spanne, uint16_t wert);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    uint16_t wert();

private:
    uint16_t _wert;
};

class Ausdruck_Ganzzahl : public Ausdruck
{
public:
    Ausdruck_Ganzzahl(Spanne spanne, uint16_t wert, uint16_t basis = 10);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    uint16_t wert() const;
    uint16_t basis() const;

private:
    uint16_t _wert;
    uint16_t _basis;
};

class Ausdruck_Text : public Ausdruck
{
public:
    Ausdruck_Text(Spanne spanne, std::string text);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    std::string text();

private:
    std::string _text;
};

class Ausdruck_Variable : public Ausdruck
{
public:
    Ausdruck_Variable(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;
    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ausdruck_Klammer : public Ausdruck
{
public:
    Ausdruck_Klammer(Spanne spanne, Ausdruck *ausdruck);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck *ausdruck();

private:
    Ausdruck *_ausdruck;
};

class Ausdruck_Als : public Ausdruck
{
public:
    Ausdruck_Als(Spanne spanne, Ausdruck *schablone, Ausdruck_Feld *variable);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck * schablone();
    Ausdruck_Feld * variable();

private:
    Ausdruck * _schablone;
    Ausdruck_Feld * _variable;
};

class Ausdruck_Feld : public Ausdruck
{
public:
    Ausdruck_Feld(Spanne spanne, Ausdruck *basis, std::string feld);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Ausdruck *kopie() override;

    Ausdruck * basis();
    std::string feld();
private:
    Ausdruck *_basis;
    std::string _feld;
};
// }}}
// anweisung {{{
#define Anweisung_Art \
    X(ASM,        1, "Asm") \
    X(MAKRO,      2, "Makro") \
    X(MARKIERUNG, 3, "Markierung") \
    X(IMPORT,     4, "Import")

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

class Anweisung_Import : public Anweisung
{
public:
    Anweisung_Import(Spanne spanne, std::string zone, uint16_t start_adresse, std::string modul);

    void ausgeben(uint8_t tiefe, std::ostream &ausgabe) override;
    Anweisung *kopie() override;

    std::string zone();
    std::string modul();
    uint16_t    start_adresse();

private:
    std::string _zone;
    uint16_t _start_adresse;
    std::string _modul;
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
