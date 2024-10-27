#pragma once

#include <cstdint>

#include "vm/anweisung.hpp"

#define Ast_Liste \
    X(AST_REG) \
    X(AST_ADRESSE) \
    X(AST_VARIABLE) \
    X(AST_ANWEISUNG) \
    X(AST_BIN) \
    X(AST_NAME) \
    X(AST_TEXT) \
    X(AST_ECKIGE_KLAMMER) \
    X(AST_KONSTANTE) \
    X(AST_HEX) \
    X(AST_GANZZAHL) \
    X(AST_KLAMMER) \
    X(AST_DATEN) \
    X(AST_SCHABLONE) \
    X(AST_ALS)

namespace Asm {

class Ast_Knoten
{
public:
    enum Ast_Art {
        #define X(Name) Name,
        Ast_Liste
        #undef X
    };

    Ast_Knoten(Ast_Art art);

    virtual void ausgeben(uint8_t tiefe = 0) = 0;

    Ast_Art art();
    template<typename T> T als();
    bool gleich(int32_t art);
    bool ungleich(int32_t art);

    bool exportieren;

protected:
    Ast_Art _art;
};

class Ast_Bin : public Ast_Knoten
{
public:
    Ast_Bin(Token *op, Ast_Knoten *links, Ast_Knoten *rechts);

    void ausgeben(uint8_t tiefe) override;

    Token *op() { return _op; }
    Ast_Knoten *links()  { return _links;  }
    Ast_Knoten *rechts() { return _rechts; }

private:
    Token *_op;
    Ast_Knoten *_links;
    Ast_Knoten *_rechts;
};

class Ast_Name : public Ast_Knoten
{
public:
    Ast_Name(Token *token);

    void ausgeben(uint8_t tiefe) override;

    char * name();

private:
    Token *_token;
};

class Ast_Eckige_Klammer : public Ast_Knoten
{
public:
    Ast_Eckige_Klammer(Token *klammer_auf, Ast_Knoten *ausdruck, Token *klammer_zu);

    void ausgeben(uint8_t tiefe) override;

    Ast_Knoten * ausdruck();

private:
    Token *_klammer_auf;
    Ast_Knoten *_ausdruck;
    Token *_klammer_zu;
};

class Ast_Reg : public Ast_Knoten
{
public:
    Ast_Reg(Token *token);

    void ausgeben(uint8_t tiefe) override;

    uint32_t reg();
    Token * token();

private:
    Token *_token;
};

class Ast_Adresse : public Ast_Knoten
{
public:
    Ast_Adresse(Token *token, Ast_Knoten *ausdruck);

    void ausgeben(uint8_t tiefe) override;
    Ast_Knoten *ausdruck();

private:
    Token *_token;
    Ast_Knoten *_ausdruck;
};

class Ast_Konstante : public Ast_Knoten
{
public:
    Ast_Konstante(Token *konst, Ast_Knoten * name, Ast_Knoten *wert);

    void ausgeben(uint8_t tiefe) override;

    Ast_Knoten *wert() { return _wert; }
    Ast_Knoten *name() { return _name; }

private:
    Token * _konst;
    Ast_Knoten * _name;
    Ast_Knoten * _wert;
};

class Ast_Hex : public Ast_Knoten
{
public:
    Ast_Hex(Token *token);

    void ausgeben(uint8_t tiefe) override;

    uint16_t wert() { return _token->als<Token_Hex *>()->zahl(); }

private:
    Token *_token;
};

class Ast_Text : public Ast_Knoten
{
public:
    Ast_Text(Token *token);

    void ausgeben(uint8_t tiefe) override;

private:
    Token *_token;
};

class Ast_Variable : public Ast_Knoten
{
public:
    Ast_Variable(Token *ausrufezeichen, Ast_Knoten *name);

    void ausgeben(uint8_t tiefe) override;
    const char * name();

private:
    Token *_ausrufezeichen;
    Ast_Knoten *_name;
};

class Ast_Ganzzahl : public Ast_Knoten
{
public:
    Ast_Ganzzahl(Token *token);

    void ausgeben(uint8_t tiefe) override;

    uint32_t wert();

private:
    Token *_token;
};

class Ast_Klammer : public Ast_Knoten
{
public:
    Ast_Klammer(Token *klammer_auf, Ast_Knoten *ausdruck, Token *klammer_zu);

    void ausgeben(uint8_t tiefe) override;

    Ast_Knoten *ausdruck() { return _ausdruck; }
    Ast_Knoten *op();

private:
    Token *_klammer_auf;
    Ast_Knoten *_ausdruck;
    Token *_klammer_zu;
};

class Ast_Anweisung : public Ast_Knoten
{
public:
    Ast_Anweisung(Ast_Knoten *markierung, Ast_Knoten *op, std::vector<Ast_Knoten *> operanden);

    void ausgeben(uint8_t tiefe) override;

    Ast_Knoten *op();
    std::vector<Ast_Knoten *> operanden();
    Ast_Knoten *markierung();

    uint32_t größe();
    Vm::Anweisung *anweisung();
    void anweisung_setzen(Vm::Anweisung *anweisung);

    uint16_t adresse;

private:
    Ast_Knoten *_markierung;
    Ast_Knoten *_op;
    std::vector<Ast_Knoten *> _operanden;
    Vm::Anweisung *_anweisung;
};

class Ast_Daten : public Ast_Knoten
{
public:
    Ast_Daten(Token *datentyp, Ast_Knoten *name, Token *gleich, Token *klammer_auf, Token *klammer_zu, uint16_t z_daten, std::vector<Ast_Knoten *> daten);

    void ausgeben(uint8_t tiefe) override;

    Ast_Knoten * name() { return _name; }
    std::vector<Ast_Knoten *> daten() { return _daten; }
    uint16_t z_daten() { return _z_daten; }
    uint16_t anz_daten() { return (uint16_t)_daten.size(); }
    uint32_t größe();

    uint16_t adresse;

private:
    Token *_datentyp;
    Ast_Knoten *_name;
    Token *_gleich;
    Token *_klammer_auf;
    Token *_klammer_zu;
    uint16_t _z_daten;
    std::vector<Ast_Knoten *> _daten;
};

class Ast_Schablone : public Ast_Knoten
{
public:
    class Feld
    {
    public:
        Feld(Ast_Knoten *name, Ast_Knoten *größe) : _name(name), _größe(größe) {}

        const char *name() { return _name->als<Ast_Name *>()->name(); }
        uint16_t größe() { return _größe->als<Ast_Hex *>()->wert(); }

    private:
        Ast_Knoten *_name;
        Ast_Knoten *_größe;
    };

    Ast_Schablone(Token *schablone, Ast_Knoten *name, Token *klammer_auf, std::vector<Feld *> felder, Token *klammer_zu);

    void ausgeben(uint8_t tiefe) override;
    std::vector<Feld *> felder();
    const char *name();

private:
    Token *_schablone;
    Ast_Knoten *_name;
    Token *_klammer_auf;
    std::vector<Feld *> _felder;
    Token *_klammer_zu;
};

class Ast_Als : public Ast_Knoten
{
public:
    Ast_Als(Token *kleiner_als, Ast_Knoten *schablone, Token *größer_als, Ast_Knoten *basis, Ast_Knoten *feld);
    void ausgeben(uint8_t tiefe) override;

    const char *schablone();
    const char *basis();
    const char *feld();

private:
    Token *_kleiner_als;
    Token *_größer_als;
    Ast_Knoten *_schablone;
    Ast_Knoten *_basis;
    Ast_Knoten *_feld;
};

}
