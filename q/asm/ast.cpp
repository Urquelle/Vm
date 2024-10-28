#include "asm/ast.hpp"

#include "vm/cpu.hpp"

char * Ast_Namen(Asm::Deklaration::Art art);
char * Ast_Namen(Asm::Ausdruck::Art art);
void Einschub_Ausgeben(uint8_t tiefe);
uint32_t Register_Id(const char * name);

namespace Asm {

// deklaration {{{
Deklaration::Deklaration(Deklaration::Art art, std::string name, bool exportieren)
    : _art(art)
    , _name(name)
    , _exportieren(exportieren)
{
}

Deklaration::Art
Deklaration::art()
{
    return _art;
}

std::string
Deklaration::name()
{
    return _name;
}

bool
Deklaration::exportieren()
{
    return _exportieren;
}

template<typename T>
T Deklaration::als()
{
    return static_cast<T> (this);
}

Ast_Konstante::Ast_Konstante(std::string name, uint16_t wert, bool exportieren)
    : Deklaration(Deklaration::KONSTANTE, name, exportieren)
    , _wert(wert)
{
}

void
Ast_Konstante::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(art()));
    /*_name->ausgeben(tiefe+1);*/
    /*_wert->ausgeben(tiefe+1);*/
    printf("\n");
}

Ast_Daten::Ast_Daten(uint16_t größe, std::string name, uint16_t anzahl, std::vector<Ast_Hex *> daten,
                     bool exportieren)
    : Deklaration(Deklaration::DATEN, name, exportieren)
    , _größe(größe)
    , _anzahl(anzahl)
    , _daten(daten)
{
}

void
Ast_Daten::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(art()));
    for (auto d : _daten)
    {
        d->ausgeben(tiefe+1);
    }
    printf("\n");
}

uint16_t
Ast_Daten::anzahl()
{
    return _anzahl;
}

uint16_t
Ast_Daten::größe()
{
    return _größe;
}

uint32_t
Ast_Daten::gesamtgröße()
{
    uint32_t erg = _größe * _anzahl;

    return erg;
}

std::vector<Ast_Hex *>
Ast_Daten::daten()
{
    return _daten;
}

Ast_Schablone::Ast_Schablone(std::string name, std::vector<Ast_Schablone::Feld *> felder)
    : Deklaration(Deklaration::SCHABLONE, name)
    , _felder(felder)
{
}

void
Ast_Schablone::ausgeben(uint8_t tiefe)
{
    //
}

std::vector<Ast_Schablone::Feld *>
Ast_Schablone::felder()
{
    return _felder;
}
// }}}
// ausdruck {{{
Ausdruck::Art
Ausdruck::art()
{
    return _art;
}

bool
Ausdruck::ungleich(Art art)
{
    auto erg = _art != art;

    return erg;
}

bool
Ausdruck::gleich(Art art)
{
    auto erg = _art == art;

    return erg;
}

template<typename T>
T Ausdruck::als()
{
    return static_cast<T> (this);
}

Ast_Als::Ast_Als(std::string schablone, std::string basis, std::string feld)
    : Ausdruck(Ausdruck::ALS)
    , _schablone(schablone)
    , _basis(basis)
    , _feld(feld)
{
}

void
Ast_Als::ausgeben(uint8_t tiefe)
{
    //
}

std::string
Ast_Als::schablone()
{
    return _schablone;
}

std::string
Ast_Als::basis()
{
    return _basis;
}

std::string
Ast_Als::feld()
{
    return _feld;
}

Ast_Bin::Ast_Bin(Token *op, Ausdruck *links, Ausdruck *rechts)
    : Ausdruck(Ausdruck::BIN)
    , _op(op)
    , _links(links)
    , _rechts(rechts)
{
}

void
Ast_Bin::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s\n", Ast_Namen(art()));
    _links->ausgeben(tiefe+1);
    Einschub_Ausgeben(tiefe+1);
    printf("%s: \n", Token_Namen(_op->art()));
    _rechts->ausgeben(tiefe+1);
    printf("\n");
}

Ast_Name::Ast_Name(std::string name)
    : Ausdruck(Ausdruck::NAME)
    , _name(name)
{
}

void
Ast_Name::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(art()), _name.c_str());
}

std::string
Ast_Name::name()
{
    return _name;
}

Ast_Reg::Ast_Reg(std::string name)
    : Ausdruck(Ausdruck::REG)
    , _name(name)
{
}

void
Ast_Reg::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(art()), _name.c_str());
}

uint32_t
Ast_Reg::reg()
{
    auto erg = Register_Id(_name.c_str());

    return erg;
}

Ast_Text::Ast_Text(std::string text)
    : Ausdruck(Ausdruck::TEXT)
    , _text(text)
{
}

void
Ast_Text::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(art()), _text.c_str());
}

#if 0
Ast_Ganzzahl::Ast_Ganzzahl(Token *token)
    : Ast_Knoten(AST_GANZZAHL)
    , _token(token)
{
}

void
Ast_Ganzzahl::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(_art), _token->text());
}

uint32_t
Ast_Ganzzahl::wert()
{
    auto erg = _token->als<Token_Ganzzahl *>()->zahl();

    return erg;
}
#endif

Ast_Klammer::Ast_Klammer(Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::KLAMMER)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Ast_Klammer::ausdruck()
{
    return _ausdruck;
}

void
Ast_Klammer::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s \n", Ast_Namen(art()));
    _ausdruck->ausgeben(tiefe + 1);
    printf("\n");
}

Ast_Adresse::Ast_Adresse(Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::ADRESSE)
    , _ausdruck(ausdruck)
{
}

void
Ast_Adresse::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(art()));
    _ausdruck->ausgeben(tiefe + 1);
}

Ausdruck *
Ast_Adresse::ausdruck()
{
    return _ausdruck;
}

Ast_Auswertung::Ast_Auswertung(Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::AUSWERTUNG)
    , _ausdruck(ausdruck)
{
}

void
Ast_Auswertung::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(art()));
    _ausdruck->ausgeben(tiefe+1);
}

Ausdruck *
Ast_Auswertung::ausdruck()
{
    return _ausdruck;
}

Ast_Variable::Ast_Variable(std::string name)
    : Ausdruck(Ausdruck::VARIABLE)
    , _name(name)
{
}

void
Ast_Variable::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(art()));
    /*_name->ausgeben(tiefe+1);*/
    printf("\n");
}

std::string
Ast_Variable::name()
{
    return _name;
}

Ast_Hex::Ast_Hex(uint16_t wert)
    : Ausdruck(Ausdruck::HEX)
    , _wert(wert)
{
}

uint16_t
Ast_Hex::wert()
{
    return _wert;
}

void
Ast_Hex::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: 0x%x\n", Ast_Namen(art()), wert());
}
// }}}
// anweisung {{{
Anweisung::Anweisung(Ast_Name *markierung, std::string op, std::vector<Ausdruck *> operanden)
    : _markierung(markierung)
    , _op(op)
    , _operanden(operanden)
    , _anweisung(nullptr)
{
}

void
Anweisung::ausgeben(uint8_t tiefe)
{
    if (_markierung)
    {
        _markierung->ausgeben(tiefe);
    }

    /*_op->ausgeben(tiefe);*/

    for (auto op : _operanden)
    {
        op->ausgeben(tiefe + 1);
    }
}

uint32_t
Anweisung::größe()
{
    uint32_t erg = 1;

    for (auto *op : operanden())
    {
        if (op->art() == Ausdruck::REG)
        {
            erg += 1;
        }

        else if (op->art() == Ausdruck::ADRESSE)
        {
            erg += 2;
        }

        else if (op->art() == Ausdruck::AUSWERTUNG)
        {
            erg += 2;
        }

        else if (op->art() == Ausdruck::HEX)
        {
            erg += 2;
        }

        else {
            assert(!"unbekannte art des operanden");
        }
    }

    return erg;
}

std::string
Anweisung::op()
{
    return _op;
}

std::vector<Ausdruck *>
Anweisung::operanden()
{
    return _operanden;
}

Ast_Name *
Anweisung::markierung()
{
    return _markierung;
}

Vm::Anweisung *
Anweisung::anweisung()
{
    return _anweisung;
}

void
Anweisung::anweisung_setzen(Vm::Anweisung *anweisung)
{
    _anweisung = anweisung;
}
// }}}

}

char *
Ast_Namen(Asm::Deklaration::Art art)
{
    switch (art)
    {
#define X(Name, Wert, Beschreibung) case Asm::Deklaration::Name: return (char *) Beschreibung;
        Deklaration_Art
#undef X
    }

    return (char *) "Deklaration: Unbekannt";
}

char *
Ast_Namen(Asm::Ausdruck::Art art)
{
    switch (art)
    {
#define X(Name, Wert, Beschreibung) case Asm::Ausdruck::Name: return (char *) Beschreibung;
        Ausdruck_Art
#undef X
    }

    return (char *) "Ausdruck: Unbekannt";
}

uint32_t
Register_Id(const char * name)
{
    if (strcmp(name, "r1")   == 0 || strcmp(name, "R1")  == 0) return Vm::REG_R1;
    if (strcmp(name, "r2")   == 0 || strcmp(name, "R2")  == 0) return Vm::REG_R2;
    if (strcmp(name, "r3")   == 0 || strcmp(name, "R3")  == 0) return Vm::REG_R3;
    if (strcmp(name, "r4")   == 0 || strcmp(name, "R4")  == 0) return Vm::REG_R4;
    if (strcmp(name, "r5")   == 0 || strcmp(name, "R5")  == 0) return Vm::REG_R5;
    if (strcmp(name, "r6")   == 0 || strcmp(name, "R6")  == 0) return Vm::REG_R6;
    if (strcmp(name, "r7")   == 0 || strcmp(name, "R7")  == 0) return Vm::REG_R7;
    if (strcmp(name, "r8")   == 0 || strcmp(name, "R8")  == 0) return Vm::REG_R8;
    if (strcmp(name, "sp")   == 0 || strcmp(name, "SP")  == 0) return Vm::REG_SP;
    if (strcmp(name, "fp")   == 0 || strcmp(name, "FP")  == 0) return Vm::REG_FP;
    if (strcmp(name, "ip")   == 0 || strcmp(name, "IP")  == 0) return Vm::REG_IP;
    if (strcmp(name, "acc")  == 0 || strcmp(name, "ACC") == 0) return Vm::REG_ACC;

    return 0;
}

void Einschub_Ausgeben(uint8_t tiefe)
{
    printf("%*s", tiefe*3, "");
}
