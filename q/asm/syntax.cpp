#include "asm/syntax.hpp"

#include <cassert>
#include <cctype>

namespace Asm {

Syntax::Syntax(std::vector<Token *> token)
    : _token(token)
    , _token_index(0)
{
}

std::vector<Ast_Knoten *>
Syntax::starten()
{
    std::vector<Ast_Knoten *> erg;

    auto angabe = zeile_analysieren();
    akzeptiere(Token::T_ZEILENUMBRUCH);

    while (angabe != nullptr)
    {
        erg.push_back(angabe);
        angabe = zeile_analysieren();
        akzeptiere(Token::T_ZEILENUMBRUCH);
    }

    return erg;
}

Ast_Knoten *
Syntax::zeile_analysieren()
{
    auto exportieren = akzeptiere(Token::T_PLUS);

    if (strcmp(token()->text(), "data8") == 0)
    {
        auto dekl = daten_dekl_einlesen(1, exportieren);

        return dekl;
    }
    else if (strcmp(token()->text(), "data16") == 0)
    {
        auto dekl = daten_dekl_einlesen(2, exportieren);

        return dekl;
    }
    else if (strcmp(token()->text(), "const") == 0)
    {
        auto konst = weiter();

        auto name = basis_ausdruck_einlesen();
        if (name == nullptr || name->ungleich(Ast_Knoten::AST_NAME))
        {
            return nullptr;
        }

        erwarte(Token::T_GLEICH);
        auto hex = basis_ausdruck_einlesen();
        if (hex == nullptr || (hex->ungleich(Ast_Knoten::AST_HEX) && hex->ungleich(Ast_Knoten::AST_GANZZAHL)))
        {
            return nullptr;
        }

        auto dekl = new Ast_Konstante(konst, name, hex);

        dekl->exportieren = exportieren;

        return dekl;
    }
    else if (strcmp(token()->text(), "schablone") == 0)
    {
        auto *erg = schablone_dekl_einlesen(exportieren);

        return erg;
    }
    else
    {
        auto anweisung = anweisung_einlesen();

        return anweisung;
    }
}

Ast_Knoten *
Syntax::operand_einlesen()
{
    auto erg = ausdruck_einlesen();

    return erg;
}

Ast_Knoten *
Syntax::anweisung_einlesen()
{
    Ast_Knoten *markierung = nullptr;
    auto anw = ausdruck_einlesen();

    if (anw == nullptr || anw->ungleich(Ast_Knoten::AST_NAME))
    {
        return nullptr;
    }

    if (akzeptiere(Token::T_DOPPELPUNKT))
    {
        markierung = anw;
        akzeptiere(Token::T_ZEILENUMBRUCH);
        anw = ausdruck_einlesen();
    }

    if (anw->ungleich(Ast_Knoten::AST_NAME))
    {
        return nullptr;
    }

    std::vector<Ast_Knoten *> ops;
    while (ungleich(Token::T_ZEILENUMBRUCH) && ungleich(Token::T_EOF))
    {
        auto op = operand_einlesen();

        if (op != nullptr)
        {
            ops.push_back(op);
        }
    }

    return new Ast_Anweisung(markierung, anw, ops);
}

Ast_Knoten *
Syntax::ausdruck_einlesen()
{
    auto aus = plus_ausdruck_einlesen();

    return aus;
}

Ast_Knoten *
Syntax::plus_ausdruck_einlesen()
{
    auto links = mult_ausdruck_einlesen();

    while (gleich(Token::T_PLUS) || gleich(Token::T_MINUS))
    {
        auto op = weiter();
        auto rechts = mult_ausdruck_einlesen();
        links = new Ast_Bin(op, links, rechts);
    }

    return links;
}

Ast_Knoten *
Syntax::mult_ausdruck_einlesen()
{
    auto links = basis_ausdruck_einlesen();

    while (gleich(Token::T_STERN) || gleich(Token::T_PISA))
    {
        auto op = weiter();
        auto rechts = basis_ausdruck_einlesen();
        links = new Ast_Bin(op, links, rechts);
    }

    return links;
}

Ast_Knoten *
Syntax::basis_ausdruck_einlesen()
{
    auto t = token();
    auto art = t->art();

    switch (art)
    {
        case Token::T_AUSRUFEZEICHEN:
        {
            auto ausrufezeichen = weiter();
            auto *basis = basis_ausdruck_einlesen();

            if (basis == nullptr || basis->art() != Ast_Knoten::AST_NAME)
            {
                assert(!"name erwartet.");
            }

            return new Ast_Variable(ausrufezeichen, basis);
        } break;

        case Token::T_KLEINER:
        {
            auto *kleiner_als = weiter();
            auto *schablone = brauche(Ast_Knoten::AST_NAME);
            auto *größer_als = erwarte(Token::T_GROESSER);
            auto *name = brauche(Ast_Knoten::AST_NAME);
            erwarte(Token::T_PUNKT);
            auto *feld = brauche(Ast_Knoten::AST_NAME);

            return new Ast_Als(kleiner_als, schablone, größer_als, name, feld);
        } break;

        case Token::T_ECKIGE_KLAMMER_AUF:
        {
            auto klammer_auf = weiter();
            auto aus = ausdruck_einlesen();
            auto klammer_zu = erwarte(Token::T_ECKIGE_KLAMMER_ZU);

            return new Ast_Eckige_Klammer(klammer_auf, aus, klammer_zu);
        } break;

        case Token::T_KAUFMANNSUND:
        {
            auto und = weiter();
            auto *ausdruck = ausdruck_einlesen();

            return new Ast_Adresse(und, ausdruck);
        } break;

        case Token::T_NAME:
        {
            if (   strcmp(token()->text(), "r1")  == 0
                || strcmp(token()->text(), "R1")  == 0
                || strcmp(token()->text(), "r2")  == 0
                || strcmp(token()->text(), "R2")  == 0
                || strcmp(token()->text(), "r3")  == 0
                || strcmp(token()->text(), "R3")  == 0
                || strcmp(token()->text(), "r4")  == 0
                || strcmp(token()->text(), "R4")  == 0
                || strcmp(token()->text(), "r5")  == 0
                || strcmp(token()->text(), "R5")  == 0
                || strcmp(token()->text(), "r6")  == 0
                || strcmp(token()->text(), "R6")  == 0
                || strcmp(token()->text(), "r7")  == 0
                || strcmp(token()->text(), "R7")  == 0
                || strcmp(token()->text(), "r8")  == 0
                || strcmp(token()->text(), "R8")  == 0
                || strcmp(token()->text(), "sp")  == 0
                || strcmp(token()->text(), "SP")  == 0
                || strcmp(token()->text(), "fp")  == 0
                || strcmp(token()->text(), "FP")  == 0
                || strcmp(token()->text(), "ip")  == 0
                || strcmp(token()->text(), "IP")  == 0
                || strcmp(token()->text(), "mb")  == 0
                || strcmp(token()->text(), "MB")  == 0
                || strcmp(token()->text(), "acc") == 0
                || strcmp(token()->text(), "ACC") == 0)
            {
                return new Ast_Reg(weiter());
            }

            return new Ast_Name(weiter());
        } break;

        case Token::T_HEX:
        {
            return new Ast_Hex(weiter());
        } break;

        case Token::T_TEXT:
        {
            return new Ast_Text(weiter());
        } break;

        case Token::T_GANZZAHL:
        {
            return new Ast_Ganzzahl(weiter());
        } break;

        case Token::T_RUNDE_KLAMMER_AUF:
        {
            auto klammer_auf = weiter();
            auto ausdruck = ausdruck_einlesen();
            auto klammer_zu = erwarte(Token::T_RUNDE_KLAMMER_ZU);

            return new Ast_Klammer(klammer_auf, ausdruck, klammer_zu);
        } break;
    }

    return nullptr;
}

Ast_Knoten *
Syntax::schablone_dekl_einlesen(bool exportieren)
{
    Ast_Knoten *erg = nullptr;

    auto *schablone = weiter();
    auto *name = brauche(Ast_Knoten::AST_NAME);
    auto *klammer_auf = erwarte(Token::T_GESCHWEIFTE_KLAMMER_AUF);

    std::vector<Ast_Schablone::Feld *> felder;
    while (ungleich(Token::T_GESCHWEIFTE_KLAMMER_ZU))
    {
        auto *feldname = basis_ausdruck_einlesen();
        assert(feldname->art() == Ast_Knoten::AST_NAME);

        erwarte(Token::T_DOPPELPUNKT);
        auto *wert = brauche(Ast_Knoten::AST_HEX);
        felder.push_back(new Ast_Schablone::Feld(feldname, wert));
        akzeptiere(Token::T_KOMMA);
    }

    auto *klammer_zu = erwarte(Token::T_GESCHWEIFTE_KLAMMER_ZU);

    erg = new Ast_Schablone(schablone, name, klammer_auf, felder, klammer_zu);

    return erg;
}

Ast_Knoten *
Syntax::daten_dekl_einlesen(uint32_t z_daten, bool exportieren)
{
    auto datentyp = weiter();
    auto name = basis_ausdruck_einlesen();

    if (name == nullptr || name->art() != Ast_Knoten::AST_NAME)
    {
        return nullptr;
    }

    auto gleich = erwarte(Token::T_GLEICH);
    auto klammer_auf = erwarte(Token::T_GESCHWEIFTE_KLAMMER_AUF);

    std::vector<Ast_Knoten *> daten;

    while (ungleich(Token::T_GESCHWEIFTE_KLAMMER_ZU))
    {
        auto hex = basis_ausdruck_einlesen();

        if (hex == nullptr || (hex->ungleich(Ast_Knoten::AST_HEX) && hex->ungleich(Ast_Knoten::AST_GANZZAHL)))
        {
            // AUFGABE: an dieser stelle sollte vielleicht ein fehler angezeigt werden?
            break;
        }

        daten.push_back(hex);

        akzeptiere(Token::T_KOMMA);
    }

    auto klammer_zu = erwarte(Token::T_GESCHWEIFTE_KLAMMER_ZU);

    // INFO: zeilenumbruch überspringen
    akzeptiere(Token::T_ZEILENUMBRUCH);

    auto erg = new Ast_Daten(datentyp, name, gleich, klammer_auf, klammer_zu, z_daten, daten);

    erg->exportieren = exportieren;

    return erg;
}

/* helfer {{{ */

Ast_Knoten *
Syntax::brauche(uint32_t art)
{
    auto *ausdruck = ausdruck_einlesen();
    if (ausdruck && ausdruck->art() == (int32_t) art)
    {
        return ausdruck;
    }

    return nullptr;
}

Token *
Syntax::token()
{
    if (_token_index >= _token.size())
    {
        auto erg = _token.at(_token.size() - 1);

        return erg;
    }

    auto erg = _token.at(_token_index);

    return erg;
}

Token *
Syntax::weiter()
{
    if (_token_index >= _token.size())
    {
        return _token.at(_token.size() - 1);
    }

    auto erg = _token.at(_token_index);
    _token_index += 1;

    // INFO: einfach nur um im debugger sehen zu können
    auto t = _token.at(_token_index);

    return erg;
}

Token *
Syntax::erwarte(uint32_t art)
{
    if (ungleich(art))
    {
        assert(!"anderes token wurde erwartet.");
    }

    return weiter();
}

bool
Syntax::gleich(uint32_t art)
{
    auto erg = token()->art() == art;

    return erg;
}

bool
Syntax::ungleich(uint32_t art)
{
    auto erg = !gleich(art);

    return erg;
}

bool
Syntax::akzeptiere(uint32_t art)
{
    if (gleich(art))
    {
        weiter();
        return true;
    }

    return false;
}
/* }}} */

}
