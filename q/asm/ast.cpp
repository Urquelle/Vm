#include "asm/ast.hpp"

#include "vm/cpu.hpp"

char * Ast_Namen(uint16_t ast_id);
void Einschub_Ausgeben(uint8_t tiefe);
uint32_t Register_Id(char * name);

namespace Asm {

Ast_Knoten::Ast_Knoten(Ast_Art art)
    : _art(art)
    , exportieren(false)
{
}

Ast_Knoten::Ast_Art
Ast_Knoten::art()
{
    return _art;
}

template<typename T>
T Ast_Knoten::als()
{
    return static_cast<T> (this);
}

bool
Ast_Knoten::gleich(int32_t art)
{
    auto erg = _art == art;

    return erg;
}

bool
Ast_Knoten::ungleich(int32_t art)
{
    auto erg = !gleich(art);

    return erg;
}

Ast_Bin::Ast_Bin(Token *op, Ast_Knoten *links, Ast_Knoten *rechts)
    : Ast_Knoten(AST_BIN)
    , _op(op)
    , _links(links)
    , _rechts(rechts)
{
}

void
Ast_Bin::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s\n", Ast_Namen(_art));
    _links->ausgeben(tiefe+1);
    Einschub_Ausgeben(tiefe+1);
    printf("%s: \n", Token_Namen(_op->art()));
    _rechts->ausgeben(tiefe+1);
    printf("\n");
}

Ast_Name::Ast_Name(Token *token)
    : Ast_Knoten(AST_NAME)
    , _token(token)
{
}

void
Ast_Name::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(_art), _token->text());
}

char *
Ast_Name::name()
{
    return _token->text();
}

Ast_Reg::Ast_Reg(Token *token)
    : Ast_Knoten(AST_REG)
    , _token(token)
{
}

void
Ast_Reg::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(_art), _token->text());
}

uint32_t
Ast_Reg::reg()
{
    auto erg = Register_Id(_token->text());

    return erg;
}

Ast_Text::Ast_Text(Token *token)
    : Ast_Knoten(AST_TEXT)
    , _token(token)
{
}

void
Ast_Text::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: %s\n", Ast_Namen(_art), _token->text());
}

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

Ast_Klammer::Ast_Klammer(Token *klammer_auf, Ast_Knoten *ausdruck, Token *klammer_zu)
    : Ast_Knoten(AST_KLAMMER)
    , _klammer_auf(klammer_auf)
    , _ausdruck(ausdruck)
    , _klammer_zu(klammer_zu)
{
}

void
Ast_Klammer::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s \n", Ast_Namen(_art));
    _ausdruck->ausgeben(tiefe + 1);
    printf("\n");
}

Ast_Adresse::Ast_Adresse(Token *token, Ast_Knoten *ausdruck)
    : Ast_Knoten(AST_ADRESSE)
    , _token(token)
    , _ausdruck(ausdruck)
{
}

void
Ast_Adresse::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(_art));
    _ausdruck->ausgeben(tiefe + 1);
}

Ast_Knoten *
Ast_Adresse::ausdruck()
{
    return _ausdruck;
}

Ast_Eckige_Klammer::Ast_Eckige_Klammer(Token *klammer_auf, Ast_Knoten *ausdruck, Token *klammer_zu)
    : Ast_Knoten(AST_ECKIGE_KLAMMER)
    , _klammer_auf(klammer_auf)
    , _ausdruck(ausdruck)
    , _klammer_zu(klammer_zu)
{
}

void
Ast_Eckige_Klammer::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(_art));
    _ausdruck->ausgeben(tiefe+1);
}

Ast_Knoten *
Ast_Eckige_Klammer::ausdruck()
{
    return _ausdruck;
}

Ast_Variable::Ast_Variable(Token *ausrufezeichen, Ast_Knoten *name)
    : Ast_Knoten(AST_VARIABLE)
    , _ausrufezeichen(ausrufezeichen)
    , _name(name)
{
}

void
Ast_Variable::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(_art));
    _name->ausgeben(tiefe+1);
    printf("\n");
}

const char *
Ast_Variable::name()
{
    assert(_name != nullptr);
    assert(_name->art() == Ast_Knoten::AST_NAME);

    return _name->als<Ast_Name *>()->name();
}

Ast_Konstante::Ast_Konstante(Token *konst, Ast_Knoten *name, Ast_Knoten *wert)
    : Ast_Knoten(AST_KONSTANTE)
    , _konst(konst)
    , _name(name)
    , _wert(wert)
{
}

void
Ast_Konstante::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(_art));
    _name->ausgeben(tiefe+1);
    _wert->ausgeben(tiefe+1);
    printf("\n");
}

Ast_Hex::Ast_Hex(Token *token)
    : Ast_Knoten(AST_HEX)
    , _token(token)
{
}

void
Ast_Hex::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: 0x%x\n", Ast_Namen(_art), wert());
}

Ast_Anweisung::Ast_Anweisung(Ast_Knoten *markierung, Ast_Knoten *op, std::vector<Ast_Knoten *> operanden)
    : Ast_Knoten(AST_ANWEISUNG)
    , _markierung(markierung)
    , _op(op)
    , _operanden(operanden)
    , _anweisung(nullptr)
{
}

void
Ast_Anweisung::ausgeben(uint8_t tiefe)
{
    if (_markierung)
    {
        _markierung->ausgeben(tiefe);
    }

    _op->ausgeben(tiefe);

    for (auto op : _operanden)
    {
        op->ausgeben(tiefe + 1);
    }
}

uint32_t
Ast_Anweisung::größe()
{
    uint32_t erg = 1;

    for (auto *op : operanden())
    {
        if (op->art() == Ast_Knoten::AST_REG)
        {
            erg += 1;
        }

        else if (op->art() == Ast_Knoten::AST_ADRESSE)
        {
            erg += 2;
        }

        else if (op->art() == Ast_Knoten::AST_ECKIGE_KLAMMER)
        {
            erg += 2;
        }

        else if (op->art() == Ast_Knoten::AST_HEX)
        {
            erg += 2;
        }

        else {
            assert(!"unbekannte art des operanden");
        }
    }

    return erg;
}

Ast_Knoten *
Ast_Anweisung::op()
{
    return _op;
}

std::vector<Ast_Knoten *>
Ast_Anweisung::operanden()
{
    return _operanden;
}

Ast_Knoten *
Ast_Anweisung::markierung()
{
    return _markierung;
}

Vm::Anweisung *
Ast_Anweisung::anweisung()
{
    return _anweisung;
}

void
Ast_Anweisung::anweisung_setzen(Vm::Anweisung *anweisung)
{
    _anweisung = anweisung;
}

Ast_Daten::Ast_Daten(Token *datentyp, Ast_Knoten *name,
                     Token *gleich, Token *klammer_auf, Token *klammer_zu, uint16_t z_daten, std::vector<Ast_Knoten *> daten)
    : Ast_Knoten(AST_DATEN)
    , _datentyp(datentyp)
    , _name(name)
    , _gleich(gleich)
    , _klammer_auf(klammer_auf)
    , _klammer_zu(klammer_zu)
    , _z_daten(z_daten)
    , _daten(daten)
{
}

void
Ast_Daten::ausgeben(uint8_t tiefe)
{
    Einschub_Ausgeben(tiefe);
    printf("%s: \n", Ast_Namen(_art));
    for (auto d : _daten)
    {
        d->ausgeben(tiefe+1);
    }
    printf("\n");
}

uint32_t
Ast_Daten::größe()
{
    uint32_t erg = _z_daten * (uint32_t) _daten.size();

    return erg;
}

Ast_Schablone::Ast_Schablone(Token *schablone, Ast_Knoten *name, Token *klammer_auf,
                             std::vector<Ast_Schablone::Feld *> felder, Token *klammer_zu)
    : Ast_Knoten(AST_SCHABLONE)
    , _schablone(schablone)
    , _name(name)
    , _klammer_auf(klammer_auf)
    , _felder(felder)
    , _klammer_zu(klammer_zu)
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

const char *
Ast_Schablone::name()
{
    return _name->als<Ast_Name *>()->name();
}

Ast_Als::Ast_Als(Token *kleiner_als, Ast_Knoten *schablone, Token *größer_als, Ast_Knoten *basis, Ast_Knoten *feld)
    : Ast_Knoten(AST_ALS)
    , _kleiner_als(kleiner_als)
    , _schablone(schablone)
    , _größer_als(größer_als)
    , _basis(basis)
    , _feld(feld)
{
}

void
Ast_Als::ausgeben(uint8_t tiefe)
{
    //
}

const char *
Ast_Als::schablone()
{
    return _schablone->als<Ast_Name *>()->name();
}

const char *
Ast_Als::basis()
{
    return _basis->als<Ast_Name *>()->name();
}

const char *
Ast_Als::feld()
{
    return _feld->als<Ast_Name *>()->name();
}

}

uint32_t
Register_Id(char * name)
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

char *
Ast_Namen(uint16_t ast_id)
{
    using namespace Asm;

    switch (ast_id)
    {
    #define X(Name) case Ast_Knoten::Name: return (char *) #Name;
    Ast_Liste
    #undef X
    }

    return (char *) "unbekannt";
}

void Einschub_Ausgeben(uint8_t tiefe)
{
    printf("%*s", tiefe*3, "");
}
