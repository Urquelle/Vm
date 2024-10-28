#include "asm/syntax.hpp"

#include <cassert>
#include <cctype>

namespace Asm {

Syntax::Zeile
Syntax::zeile_deklaration(Asm::Deklaration *dekl)
{
    Zeile erg = {
        .art = Syntax::Deklaration,
        .daten = dekl
    };

    return erg;
}

Syntax::Zeile
Syntax::zeile_anweisung(Asm::Anweisung *anweisung)
{
    Zeile erg = {
        .art = Syntax::Anweisung,
        .daten = anweisung
    };

    return erg;
}

Syntax::Syntax(std::vector<Token *> token)
    : _token(token)
    , _token_index(0)
{
}

Ast
Syntax::starten()
{
    Ast erg;

    auto zeile = zeile_einlesen();
    akzeptiere(Token::ZEILENUMBRUCH);

    while (zeile.daten != nullptr)
    {
        if (zeile.art == Syntax::Deklaration)
        {
            erg.deklarationen.push_back((Asm::Deklaration *) zeile.daten);
        }
        else if (zeile.art == Syntax::Anweisung)
        {
            erg.anweisungen.push_back((Asm::Anweisung *) zeile.daten);
        }
        else
        {
            assert(!"unbekannt");
        }

        zeile = zeile_einlesen();
        akzeptiere(Token::ZEILENUMBRUCH);
    }

    return erg;
}

Syntax::Zeile
Syntax::zeile_einlesen()
{
    auto exportieren = akzeptiere(Token::PLUS);

    if (strcmp(token()->text(), "data8") == 0)
    {
        auto dekl = daten_dekl_einlesen(1, exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "data16") == 0)
    {
        auto dekl = daten_dekl_einlesen(2, exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "const") == 0)
    {
        auto konst = weiter();
        auto *name = brauche<Name *>(Ausdruck::NAME);
        erwarte(Token::GLEICH);
        auto *hex = brauche<Hex *>(Ausdruck::HEX);

        if (name == nullptr || hex == nullptr)
        {
            assert(!"name und wert müssen vorhanden sein.");
        }

        auto dekl = new Konstante(
            name->name(),
            hex->wert(),
            exportieren);

        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "schablone") == 0)
    {
        auto *dekl = schablone_dekl_einlesen(exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else
    {
        auto *anweisung = anweisung_einlesen();
        auto erg = zeile_anweisung(anweisung);

        return erg;
    }
}

Asm::Ausdruck *
Syntax::operand_einlesen()
{
    auto erg = ausdruck_einlesen();

    return erg;
}

Asm::Anweisung *
Syntax::anweisung_einlesen()
{
    Name *markierung = nullptr;
    auto *anw = brauche<Name *>(Ausdruck::NAME);

    if (anw == nullptr)
    {
        return nullptr;
    }

    if (akzeptiere(Token::DOPPELPUNKT))
    {
        markierung = anw;
        akzeptiere(Token::ZEILENUMBRUCH);
        anw = brauche<Name *>(Ausdruck::NAME);
    }

    if (anw == nullptr)
    {
        return nullptr;
    }

    std::vector<Ausdruck *> operanden;
    while (ungleich(Token::ZEILENUMBRUCH) && ungleich(Token::ENDE))
    {
        auto *operand = operand_einlesen();

        if (operand != nullptr)
        {
            operanden.push_back(operand);
        }
    }

    return new Asm::Anweisung(markierung, anw->name(), operanden);
}

Ausdruck *
Syntax::ausdruck_einlesen()
{
    auto *aus = plus_ausdruck_einlesen();

    return aus;
}

Ausdruck *
Syntax::plus_ausdruck_einlesen()
{
    auto *links = mult_ausdruck_einlesen();

    while (gleich(Token::PLUS) || gleich(Token::MINUS))
    {
        auto op = weiter();
        auto *rechts = mult_ausdruck_einlesen();
        links = new Bin(op, links, rechts);
    }

    return links;
}

Ausdruck *
Syntax::mult_ausdruck_einlesen()
{
    auto *links = basis_ausdruck_einlesen();

    while (gleich(Token::STERN) || gleich(Token::PISA))
    {
        auto op = weiter();
        auto *rechts = basis_ausdruck_einlesen();
        links = new Bin(op, links, rechts);
    }

    return links;
}

Ausdruck *
Syntax::basis_ausdruck_einlesen()
{
    auto t = token();
    auto art = t->art();

    switch (art)
    {
        case Token::AUSRUFEZEICHEN:
        {
            auto ausrufezeichen = weiter();
            auto *ausdruck = brauche<Name *>(Ausdruck::NAME);

            if (ausdruck == nullptr)
            {
                assert(!"name erwartet.");
            }

            return new Variable(ausdruck->name());
        } break;

        case Token::KLEINER:
        {
            auto *kleiner_als = weiter();
            auto *schablone = brauche<Name *>(Ausdruck::NAME);
            auto *größer_als = erwarte(Token::GROESSER);
            auto *basis = brauche<Name *>(Ausdruck::NAME);
            erwarte(Token::PUNKT);
            auto *feld = brauche<Name *>(Ausdruck::NAME);

            return new Als(schablone->name(), basis->name(), feld->name());
        } break;

        case Token::ECKIGE_KLAMMER_AUF:
        {
            weiter();
            auto *ausdruck = ausdruck_einlesen();
            erwarte(Token::ECKIGE_KLAMMER_ZU);

            return new Auswertung(ausdruck);
        } break;

        case Token::KAUFMANNSUND:
        {
            weiter();
            auto *ausdruck = ausdruck_einlesen();

            return new Adresse(ausdruck);
        } break;

        case Token::NAME:
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
                return new Reg(weiter()->text());
            }

            return new Name(weiter()->text());
        } break;

        case Token::HEX:
        {
            return new Hex(weiter()->als<Token_Hex *>()->zahl());
        } break;

        case Token::TEXT:
        {
            return new Text(weiter()->text());
        } break;

        case Token::GANZZAHL:
        {
            /*return new Ganzzahl(weiter());*/
        } break;

        case Token::RUNDE_KLAMMER_AUF:
        {
            weiter();
            auto ausdruck = ausdruck_einlesen();
            erwarte(Token::RUNDE_KLAMMER_ZU);

            return new Klammer(ausdruck);
        } break;
    }

    return nullptr;
}

Asm::Deklaration *
Syntax::schablone_dekl_einlesen(bool exportieren)
{
    Asm::Deklaration *erg = nullptr;

    weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);

    akzeptiere(Token::ZEILENUMBRUCH);
    erwarte(Token::GESCHWEIFTE_KLAMMER_AUF);
    akzeptiere(Token::ZEILENUMBRUCH);

    std::vector<Schablone::Feld *> felder;
    while (ungleich(Token::GESCHWEIFTE_KLAMMER_ZU))
    {
        auto *feldname = brauche<Name *>(Ausdruck::NAME);
        erwarte(Token::DOPPELPUNKT);
        auto *wert = brauche<Hex *>(Ausdruck::HEX);
        felder.push_back(new Schablone::Feld(feldname->name(), wert->wert()));

        akzeptiere(Token::KOMMA);
        akzeptiere(Token::ZEILENUMBRUCH);
    }

    erwarte(Token::GESCHWEIFTE_KLAMMER_ZU);
    akzeptiere(Token::ZEILENUMBRUCH);

    erg = new Schablone(name->name(), felder);

    return erg;
}

Asm::Deklaration *
Syntax::daten_dekl_einlesen(uint32_t größe, bool exportieren)
{
    weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);

    if (name == nullptr)
    {
        return nullptr;
    }

    erwarte(Token::GLEICH);
    erwarte(Token::GESCHWEIFTE_KLAMMER_AUF);

    std::vector<Hex *> daten;
    while (ungleich(Token::GESCHWEIFTE_KLAMMER_ZU))
    {
        auto hex = brauche<Hex *>(Ausdruck::HEX);

        if (hex == nullptr)
        {
            assert(!"hex ausdruck erwartet");
        }

        daten.push_back(hex);
        akzeptiere(Token::KOMMA);
    }

    erwarte(Token::GESCHWEIFTE_KLAMMER_ZU);

    // INFO: zeilenumbruch überspringen
    akzeptiere(Token::ZEILENUMBRUCH);

    auto erg = new Daten(größe, name->name(), (uint16_t) daten.size(), daten, exportieren);

    return erg;
}

/* helfer {{{ */

template<typename T> T
Syntax::brauche(Asm::Ausdruck::Art art)
{
    auto *ausdruck = ausdruck_einlesen();
    if (ausdruck && ausdruck->gleich(art))
    {
        return ausdruck->als<T>();
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
Syntax::erwarte(Token::Art art)
{
    if (ungleich(art))
    {
        assert(!"anderes token wurde erwartet.");
    }

    return weiter();
}

bool
Syntax::gleich(Token::Art art)
{
    auto erg = token()->art() == art;

    return erg;
}

bool
Syntax::ungleich(Token::Art art)
{
    auto erg = !gleich(art);

    return erg;
}

bool
Syntax::akzeptiere(Token::Art art)
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
