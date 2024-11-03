#include "asm/ast.hpp"

#include "ast.hpp"
#include "vm/cpu.hpp"
#include <string>

char * Ast_Namen(Asm::Deklaration::Art art);
char * Ast_Namen(Asm::Ausdruck::Art art);
char * Ast_Namen(Asm::Anweisung::Art art);

void Einschub_Ausgeben(uint8_t tiefe, std::ostream &ausgabe);
uint32_t Register_Id(const char * name);

namespace Asm {

// deklaration {{{
Deklaration::Deklaration(Deklaration::Art art, Spanne spanne, std::string name, bool exportieren)
    : _art(art)
    , _spanne(spanne)
    , _name(name)
    , _exportieren(exportieren)
{
}

Deklaration::Art
Deklaration::art() const
{
    return _art;
}

Spanne
Deklaration::spanne() const
{
    return _spanne;
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

Deklaration_Konstante::Deklaration_Konstante(Spanne spanne, std::string name, uint16_t wert, bool exportieren)
    : Deklaration(Deklaration::KONSTANTE, spanne, name, exportieren)
    , _wert(wert)
{
}

void
Deklaration_Konstante::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
    /*_name->ausgeben(tiefe+1);*/
    /*_wert->ausgeben(tiefe+1);*/
}

Deklaration_Daten::Deklaration_Daten(Spanne spanne, uint16_t größe, std::string name,
                                     uint16_t anzahl, std::vector<Hex *> daten, bool exportieren)
    : Deklaration(Deklaration::DATEN, spanne, name, exportieren)
    , _größe(größe)
    , _anzahl(anzahl)
    , _daten(daten)
{
}

void
Deklaration_Daten::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;

    for (auto d : _daten)
    {
        d->ausgeben(tiefe+1, ausgabe);
    }

    ausgabe << std::endl;
}

uint16_t
Deklaration_Daten::anzahl()
{
    return _anzahl;
}

uint16_t
Deklaration_Daten::größe()
{
    return _größe;
}

uint32_t
Deklaration_Daten::gesamtgröße()
{
    uint32_t erg = _größe * _anzahl;

    return erg;
}

std::vector<Hex *>
Deklaration_Daten::daten()
{
    return _daten;
}

Deklaration_Schablone::Deklaration_Schablone(Spanne spanne, std::string name,
                                             std::vector<Deklaration_Schablone::Feld *> felder)
    : Deklaration(Deklaration::SCHABLONE, spanne, name)
    , _felder(felder)
{
}

void
Deklaration_Schablone::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    ausgabe << "Schablone" << std::endl;
}

std::vector<Deklaration_Schablone::Feld *>
Deklaration_Schablone::felder()
{
    return _felder;
}

Deklaration_Makro::Deklaration_Makro(Spanne spanne, std::string name,
        std::vector<Name *> parameter, std::vector<Anweisung *> rumpf)
    : Deklaration(Deklaration::MAKRO, spanne, name)
    , _parameter(parameter)
    , _rumpf(rumpf)
{
}

std::vector<Name *>
Deklaration_Makro::parameter()
{
    return _parameter;
}

std::vector<Anweisung *>
Deklaration_Makro::rumpf()
{
    return _rumpf;
}

void
Deklaration_Makro::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << name() << std::endl;

    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << "Parameter" << std::endl;
    for (auto *param : _parameter)
    {
        param->ausgeben(tiefe + 1, ausgabe);
    }

    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << "Rumpf" << std::endl;

    for (auto *anweisung : _rumpf)
    {
        anweisung->ausgeben(tiefe + 1, ausgabe);
    }
}
// }}}
// ausdruck {{{
Ausdruck::Art
Ausdruck::art() const
{
    return _art;
}

Spanne
Ausdruck::spanne() const
{
    return _spanne;
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

Als::Als(Spanne spanne, std::string schablone, std::string basis, std::string feld)
    : Ausdruck(Ausdruck::ALS, spanne)
    , _schablone(schablone)
    , _basis(basis)
    , _feld(feld)
{
}

void
Als::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
}

Ausdruck *
Als::kopie()
{
    Als *erg = new Als(spanne(), _schablone, _basis, _feld);

    return erg;
}

std::string
Als::schablone()
{
    return _schablone;
}

std::string
Als::basis()
{
    return _basis;
}

std::string
Als::feld()
{
    return _feld;
}

Bin::Bin(Spanne spanne, Token *op, Ausdruck *links, Ausdruck *rechts)
    : Ausdruck(Ausdruck::BIN, spanne)
    , _op(op)
    , _links(links)
    , _rechts(rechts)
{
}

void
Bin::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
    _links->ausgeben(tiefe+1, ausgabe);
    Einschub_Ausgeben(tiefe+1, ausgabe);
    ausgabe << Token_Namen(_op->art()) << std::endl;
    _rechts->ausgeben(tiefe+1, ausgabe);
    ausgabe << std::endl;
}

Ausdruck *
Bin::kopie()
{
    Bin *erg = new Bin(spanne(), _op, _links->kopie(), _rechts->kopie());

    return erg;
}

Name::Name(Spanne spanne, std::string name)
    : Ausdruck(Ausdruck::NAME, spanne)
    , _name(name)
{
}

void
Name::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _name << std::endl;
}

Ausdruck *
Name::kopie()
{
    Name *erg = new Name(spanne(), _name);

    return erg;
}

std::string
Name::name()
{
    return _name;
}

Reg::Reg(Spanne spanne, std::string name)
    : Ausdruck(Ausdruck::REG, spanne)
    , _name(name)
{
}

void
Reg::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _name << std::endl;
}

Ausdruck *
Reg::kopie()
{
    Reg *erg = new Reg(spanne(), _name);

    return erg;
}

uint32_t
Reg::reg()
{
    auto erg = Register_Id(_name.c_str());

    return erg;
}

Text::Text(Spanne spanne, std::string text)
    : Ausdruck(Ausdruck::TEXT, spanne)
    , _text(text)
{
}

void
Text::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _text << std::endl;
}

Ausdruck *
Text::kopie()
{
    Text *erg = new Text(spanne(), _text);

    return erg;
}

#if 0
Ganzzahl::Ganzzahl(Token *token)
    : Knoten(AST_GANZZAHL)
    , _token(token)
{
}

void
Ganzzahl::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(_art), _token->text());
}

uint32_t
Ganzzahl::wert()
{
    auto erg = _token->als<Token_Ganzzahl *>()->zahl();

    return erg;
}
#endif

Klammer::Klammer(Spanne spanne, Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::KLAMMER, spanne)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Klammer::ausdruck()
{
    return _ausdruck;
}

void
Klammer::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
    _ausdruck->ausgeben(tiefe + 1, ausgabe);
    ausgabe << std::endl;
}

Ausdruck *
Klammer::kopie()
{
    Klammer *erg = new Klammer(spanne(), _ausdruck->kopie());

    return erg;
}

Adresse::Adresse(Spanne spanne, Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::ADRESSE, spanne)
    , _ausdruck(ausdruck)
{
}

void
Adresse::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
    _ausdruck->ausgeben(tiefe + 1, ausgabe);
}

Ausdruck *
Adresse::kopie()
{
    Adresse *erg = new Adresse(spanne(), _ausdruck->kopie());

    return erg;
}

Ausdruck *
Adresse::ausdruck()
{
    return _ausdruck;
}

Auswertung::Auswertung(Spanne spanne, Ausdruck *ausdruck)
    : Ausdruck(Ausdruck::AUSWERTUNG, spanne)
    , _ausdruck(ausdruck)
{
}

void
Auswertung::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << std::endl;
    _ausdruck->ausgeben(tiefe+1, ausgabe);
}

Ausdruck *
Auswertung::kopie()
{
    Auswertung *erg = new Auswertung(spanne(), _ausdruck->kopie());

    return erg;
}

Ausdruck *
Auswertung::ausdruck()
{
    return _ausdruck;
}

Variable::Variable(Spanne spanne, std::string name)
    : Ausdruck(Ausdruck::VARIABLE, spanne)
    , _name(name)
{
}

void
Variable::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _name << std::endl;
}

Ausdruck *
Variable::kopie()
{
    Variable *erg = new Variable(spanne(), _name);

    return erg;
}

std::string
Variable::name()
{
    return _name;
}

Hex::Hex(Spanne spanne, uint16_t wert)
    : Ausdruck(Ausdruck::HEX, spanne)
    , _wert(wert)
{
}

uint16_t
Hex::wert()
{
    return _wert;
}

void
Hex::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << " " << std::format("{:#06x}", wert()) << std::endl;
}

Ausdruck *
Hex::kopie()
{
    Hex *erg = new Hex(spanne(), _wert);

    return erg;
}
// }}}
// anweisung {{{
Anweisung::Anweisung(Anweisung::Art art, Spanne spanne)
    : _art(art)
    , _spanne(spanne)
    , _adresse(0)
    , _vm_anweisung(nullptr)
{
}

Anweisung::Art
Anweisung::art() const
{
    return _art;
}

Spanne
Anweisung::spanne() const
{
    return _spanne;
}

Vm::Anweisung *
Anweisung::vm_anweisung()
{
    return _vm_anweisung;
}

void
Anweisung::vm_anweisung_setzen(Vm::Anweisung *vm_anweisung)
{
    _vm_anweisung = vm_anweisung;
}

uint16_t
Anweisung::adresse()
{
    return _adresse;
}

void
Anweisung::adresse_setzen(uint16_t adresse)
{
    _adresse = adresse;
}

template<typename T>
T Anweisung::als()
{
    return static_cast<T> (this);
}

Anweisung_Asm::Anweisung_Asm(Spanne spanne, std::string op, std::vector<Ausdruck *> operanden)
    : Anweisung(Anweisung::ASM, spanne)
    , _op(op)
    , _operanden(operanden)
{
}

Anweisung *
Anweisung_Asm::kopie()
{
    std::vector<Ausdruck *> ops;

    for (auto *operand : operanden())
    {
        ops.push_back(operand->kopie());
    }

    Anweisung_Asm *erg = new Anweisung_Asm(spanne(), op(), ops);

    return erg;
}

void
Anweisung_Asm::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << _op << std::endl;

    for (auto op : _operanden)
    {
        op->ausgeben(tiefe + 1, ausgabe);
    }
}

uint32_t
Anweisung_Asm::größe()
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
Anweisung_Asm::op()
{
    return _op;
}

std::vector<Ausdruck *>
Anweisung_Asm::operanden()
{
    return _operanden;
}

Anweisung_Makro::Anweisung_Makro(Spanne spanne, std::string name, std::vector<Ausdruck *> argumente)
    : Anweisung(Anweisung::MAKRO, spanne)
    , _name(name)
    , _argumente(argumente)
{
}

void
Anweisung_Makro::ausgeben(uint8_t tiefe, std::ostream& ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _name << std::endl;
    Einschub_Ausgeben(tiefe+1, ausgabe);
    ausgabe << "Operanden" << std::endl;

    for (auto *argument : _argumente)
    {
        argument->ausgeben(tiefe+2, ausgabe);
    }
}

Anweisung *
Anweisung_Makro::kopie()
{
    std::vector<Ausdruck *> argumente;

    for (auto *argument : _argumente)
    {
        argumente.push_back(argument->kopie());
    }

    Anweisung_Makro *erg = new Anweisung_Makro(spanne(), _name, argumente);

    return erg;
}

std::string
Anweisung_Makro::name() const
{
    return _name;
}

std::vector<Ausdruck *>
Anweisung_Makro::argumente()
{
    return _argumente;
}

Anweisung_Markierung::Anweisung_Markierung(Spanne spanne, std::string name)
    : Anweisung(Anweisung::MARKIERUNG, spanne)
    , _name(name)
{
}

void
Anweisung_Markierung::ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    Einschub_Ausgeben(tiefe, ausgabe);
    ausgabe << Ast_Namen(art()) << ": " << _name;
}

Anweisung *
Anweisung_Markierung::kopie()
{
    Anweisung_Markierung *erg = new Anweisung_Markierung(spanne(), _name);

    return erg;
}

std::string
Anweisung_Markierung::name()
{
    return _name;
}
// }}}

}
// helfer {{{
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

char *
Ast_Namen(Asm::Anweisung::Art art)
{
    switch (art)
    {
#define X(Name, Wert, Beschreibung) case Asm::Anweisung::Name: return (char *) Beschreibung;
        Anweisung_Art
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
    if (strcmp(name, "acu")  == 0 || strcmp(name, "ACU") == 0) return Vm::REG_ACU;

    return 0;
}

void Einschub_Ausgeben(uint8_t tiefe, std::ostream &ausgabe)
{
    for (int32_t i = 0; i < tiefe*3; ++i)
    {
        ausgabe << " ";
    }
}
// }}}
