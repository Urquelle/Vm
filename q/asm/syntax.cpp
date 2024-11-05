#include "asm/syntax.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

#include "allgemein/diagnostik.hpp"
#include "asm/ast.hpp"
#include "asm/lexer.hpp"

namespace Asm {

Fehler * Syntax::Fehler_Name_Erwartet = new Fehler("gültigen bezeichner erwartet.");
Fehler * Syntax::Fehler_Wert_Erwartet = new Fehler("gültigen zahlenwert erwartet.");
Fehler * Syntax::Fehler_Token_Erwartet = new Fehler("'%d' erwartet, aber '%d' erhalten.");

Syntax::Zeile
Syntax::zeile_deklaration(Asm::Deklaration *dekl)
{
    Zeile erg = {
        .art = Syntax::DEKLARATION,
        .daten = dekl
    };

    return erg;
}

Syntax::Zeile
Syntax::zeile_anweisung(Asm::Anweisung *anweisung)
{
    Zeile erg = {
        .art = Syntax::ANWEISUNG,
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
        if (zeile.art == Syntax::DEKLARATION)
        {
            erg.deklarationen.push_back((Asm::Deklaration *) zeile.daten);
        }
        else if (zeile.art == Syntax::ANWEISUNG)
        {
            auto *anweisung = (Asm::Anweisung *) zeile.daten;

            if (anweisung->art() == Anweisung::IMPORT)
            {
                auto *import = anweisung->als<Anweisung_Import *>();
                auto *modul = new Modul();

                modul->spanne = Spanne(anweisung->spanne());
                modul->name = import->zone();
                modul->adresse = import->start_adresse();

                auto datei_name = anweisung->als<Anweisung_Import *>()->modul();
                std::ifstream t(datei_name);
                std::stringstream text;
                text << t.rdbuf();

                auto lex = Lexer(datei_name, text.str());
                auto tokens = lex.starten();

                auto syntax = Syntax(tokens);
                auto ast = syntax.starten();

                for (auto *dekl : ast.deklarationen)
                {
                    modul->deklarationen.push_back(dekl);
                }

                for (auto *a : ast.anweisungen)
                {
                    modul->anweisungen.push_back(a);
                }

                erg.module.push_back(modul);
            }
            else
            {
                erg.anweisungen.push_back(anweisung);
            }
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

Diagnostik
Syntax::diagnostik()
{
    return _diagnostik;
}

void
Syntax::melden(Spanne spanne, Fehler *fehler)
{
    _diagnostik.melden(spanne, fehler);
}

void
Syntax::melden(Token *token, Fehler *fehler)
{
    melden(token->spanne(), fehler);
}

void
Syntax::melden(Ausdruck *ausdruck, Fehler *fehler)
{
    melden(ausdruck->spanne(), fehler);
}

void
Syntax::melden(Anweisung *anweisung, Fehler *fehler)
{
    melden(anweisung->spanne(), fehler);
}

void
Syntax::melden(Deklaration *deklaration, Fehler *fehler)
{
    melden(deklaration->spanne(), fehler);
}

Syntax::Zeile
Syntax::zeile_einlesen()
{
    akzeptiere(Token::ZEILENUMBRUCH);

    auto exportieren = akzeptiere(Token::PLUS);

    if (strcmp(token()->text(), "dat8") == 0)
    {
        auto dekl = daten_dekl_einlesen(1, exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "dat16") == 0)
    {
        auto dekl = daten_dekl_einlesen(2, exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "const") == 0)
    {
        auto *dekl = konst_dekl_einlesen(exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "schablone") == 0)
    {
        auto *dekl = schablone_dekl_einlesen(exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "makro") == 0)
    {
        auto *dekl = makro_dekl_einlesen(exportieren);
        Zeile erg = zeile_deklaration(dekl);

        return erg;
    }
    else if (strcmp(token()->text(), "import") == 0)
    {
        auto *anweisung = import_anweisung_einlesen();
        Zeile erg = zeile_anweisung(anweisung);

        return erg;
    }
    else
    {
        auto *anweisung = anweisung_einlesen();
        auto erg = zeile_anweisung(anweisung);

        return erg;
    }
}

Asm::Anweisung *
Syntax::makro_anweisung_einlesen()
{
    auto *prozent = weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);
    FEHLER_WENN(name == nullptr, token(), new Fehler("namen des makros erwartet"));

    std::vector<Ausdruck *> operanden;
    while (Ausdruck *operand = ausdruck_einlesen())
    {
        operanden.push_back(operand);
    }

    return new Anweisung_Makro(
        Spanne(prozent->spanne().von(), operanden.back()->spanne().bis()),
        name->name(), operanden
    );
}

Asm::Anweisung *
Syntax::asm_anweisung_einlesen()
{
    auto *anw = brauche<Name *>(Ausdruck::NAME);
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

    auto anfang = anw->spanne().von();
    auto ende = operanden.size() ? operanden.back()->spanne().bis() : anw->spanne().bis();

    return new Asm::Anweisung_Asm(
        Spanne(anfang, ende),
        anw->name(), operanden);
}

Asm::Anweisung *
Syntax::markierung_anweisung_einlesen()
{
    auto *name = brauche<Name *>(Ausdruck::NAME);
    auto *doppelpunkt = erwarte(Token::DOPPELPUNKT);
    akzeptiere(Token::ZEILENUMBRUCH);

    return new Asm::Anweisung_Markierung(
        Spanne(
            name->spanne().von(),
            doppelpunkt->spanne().bis()
        ),
        name->name()
    );
}

Asm::Anweisung *
Syntax::anweisung_einlesen()
{
    Anweisung *anweisung = nullptr;

    if (gleich(Token::PROZENT))
    {
        anweisung = makro_anweisung_einlesen();
    }
    else if (gleich(Token::NAME) && token(1)->art() == Token::DOPPELPUNKT)
    {
        anweisung = markierung_anweisung_einlesen();
    }
    else
    {
        anweisung = asm_anweisung_einlesen();
    }

    return anweisung;
}

Asm::Anweisung *
Syntax::import_anweisung_einlesen()
{
    Asm::Anweisung *erg = nullptr;

    auto *schlüsselwort = brauche<Name *>(Ausdruck::NAME);
    FEHLER_WENN(schlüsselwort == nullptr, token(), new Fehler("schlüsselwort 'import' erwartet"));
    FEHLER_WENN(strcmp("import", schlüsselwort->name().c_str()) != 0, schlüsselwort, new Fehler("schlüsselwort 'import' erwartet"));

    auto *zone = brauche<Name *>(Ausdruck::NAME);
    FEHLER_WENN(zone == nullptr, token(), new Fehler("gültigen namen erwartet"));

    auto *adresse = brauche<Hex *>(Ausdruck::HEX);
    FEHLER_WENN(adresse == nullptr, token(), new Fehler("hexliteral als adresse erwartet"));

    auto *modul = brauche<Text *>(Ausdruck::TEXT);
    FEHLER_WENN(modul == nullptr, token(), new Fehler("\"<dateiname>\" erwartet"));

    akzeptiere(Token::ZEILENUMBRUCH);

    erg = new Anweisung_Import(
        Spanne(schlüsselwort->spanne().von(), modul->spanne().bis()),
        zone->name(),
        adresse->wert(),
        modul->text());

    return erg;
}

Asm::Ausdruck *
Syntax::operand_einlesen()
{
    auto erg = ausdruck_einlesen();

    return erg;
}
// ausdruck {{{
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
        links = new Bin(Spanne(links->spanne().von(), rechts->spanne().bis()),
                        op, links, rechts);
    }

    return links;
}

Ausdruck *
Syntax::mult_ausdruck_einlesen()
{
    auto *links = feld_ausdruck_einlesen();

    while (gleich(Token::STERN) || gleich(Token::PISA))
    {
        auto op = weiter();
        auto *rechts = basis_ausdruck_einlesen();
        links = new Bin(Spanne(links->spanne().von(), rechts->spanne().bis()), op, links, rechts);
    }

    return links;
}

Ausdruck *
Syntax::feld_ausdruck_einlesen()
{
    auto *links = basis_ausdruck_einlesen();

    while (akzeptiere(Token::PUNKT))
    {
        auto *feld = brauche<Name *>(Ausdruck::NAME);
        FEHLER_WENN(feld == nullptr, token(), new Fehler("gültigen bezeichner erwartet"));

        links = new Feld(
            Spanne(
                links->spanne().von(), feld->spanne().bis()
            ), links, feld->name());
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
            auto *ausrufezeichen = weiter();
            auto *ausdruck = ausdruck_einlesen();

            return new Variable(Spanne(ausrufezeichen->spanne().von(), ausdruck->spanne().bis()), ausdruck);
        } break;

        case Token::KLEINER:
        {
            auto *kleiner_als = weiter();
            auto *schablone = ausdruck_einlesen();
            auto *größer_als = erwarte(Token::GRÖSSER);
            auto *variable = brauche<Feld *>(Ausdruck::FELD);
            FEHLER_WENN(variable == nullptr, token(), new Fehler("feldzugriff erwartet"));

            return new Als(
                Spanne(kleiner_als->spanne().von(), variable->spanne().bis()),
                schablone, variable);
        } break;

        case Token::ECKIGE_KLAMMER_AUF:
        {
            auto *klammer_auf = weiter();
            auto *ausdruck = ausdruck_einlesen();
            auto *klammer_zu = erwarte(Token::ECKIGE_KLAMMER_ZU);

            return new Auswertung(Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
                                  ausdruck);
        } break;

        case Token::KAUFMANNSUND:
        {
            auto *und = weiter();
            auto *ausdruck = ausdruck_einlesen();

            return new Adresse(Spanne(und->spanne().von(), ausdruck->spanne().bis()),
                               ausdruck);
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
                || strcmp(token()->text(), "acu") == 0
                || strcmp(token()->text(), "ACU") == 0)
            {
                auto *reg = weiter();
                return new Reg(reg->spanne(), reg->text());
            }

            auto *name = weiter();
            return new Name(name->spanne(), name->text());
        } break;

        case Token::HEX:
        {
            auto *token = weiter();
            return new Hex(token->spanne(), token->als<Token_Hex *>()->wert());
        } break;

        case Token::TEXT:
        {
            auto *token = weiter();
            return new Text(token->spanne(), token->text());
        } break;

        case Token::GANZZAHL:
        {
            /*return new Ganzzahl(weiter());*/
        } break;

        case Token::RUNDE_KLAMMER_AUF:
        {
            auto *klammer_auf = weiter();
            auto ausdruck = ausdruck_einlesen();
            auto *klammer_zu = erwarte(Token::RUNDE_KLAMMER_ZU);

            return new Klammer(Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
                               ausdruck);
        } break;
    }

    return nullptr;
}
// }}}
// deklaration {{{
Asm::Deklaration *
Syntax::konst_dekl_einlesen(bool exportieren)
{
    auto *konst = weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);
    FEHLER_WENN(name == nullptr, token(), Syntax::Fehler_Name_Erwartet);
    erwarte(Token::GLEICH);
    auto *hex = brauche<Hex *>(Ausdruck::HEX);
    FEHLER_WENN(hex == nullptr, token(), Syntax::Fehler_Wert_Erwartet);

    auto *erg = new Deklaration_Konstante(
        Spanne(konst->spanne().von(), hex->spanne().bis()),
        name->name(),
        hex->wert(),
        exportieren);

    return erg;
}

Asm::Deklaration *
Syntax::schablone_dekl_einlesen(bool exportieren)
{
    Asm::Deklaration *erg = nullptr;

    auto *anfang = weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);

    akzeptiere(Token::ZEILENUMBRUCH);
    erwarte(Token::GESCHWEIFTE_KLAMMER_AUF);
    akzeptiere(Token::ZEILENUMBRUCH);

    std::vector<Deklaration_Schablone::Feld *> felder;
    while (ungleich(Token::GESCHWEIFTE_KLAMMER_ZU))
    {
        auto *feldname = brauche<Name *>(Ausdruck::NAME);
        erwarte(Token::DOPPELPUNKT);
        auto *wert = brauche<Hex *>(Ausdruck::HEX);
        felder.push_back(new Deklaration_Schablone::Feld(
            Spanne(feldname->spanne().von(), wert->spanne().bis()),
            feldname->name(), wert->wert()));

        akzeptiere(Token::KOMMA);
        akzeptiere(Token::ZEILENUMBRUCH);
    }

    auto *klammer_zu = erwarte(Token::GESCHWEIFTE_KLAMMER_ZU);
    akzeptiere(Token::ZEILENUMBRUCH);

    erg = new Deklaration_Schablone(Spanne(anfang->spanne().von(), klammer_zu->spanne().bis()),
                                    name->name(), felder);

    return erg;
}

Asm::Deklaration *
Syntax::makro_dekl_einlesen(bool exportieren)
{
    Asm::Deklaration *erg = nullptr;

    auto *anfang = weiter();
    auto *name = brauche<Name *>(Ausdruck::NAME);

    erwarte(Token::RUNDE_KLAMMER_AUF);
    std::vector<Name *> parameter;
    while (ungleich(Token::RUNDE_KLAMMER_ZU))
    {
        auto *parameter_name = brauche<Name *>(Ausdruck::NAME);
        parameter.push_back(parameter_name);

        akzeptiere(Token::KOMMA);
        akzeptiere(Token::ZEILENUMBRUCH);
    }

    erwarte(Token::RUNDE_KLAMMER_ZU);
    akzeptiere(Token::ZEILENUMBRUCH);

    erwarte(Token::GESCHWEIFTE_KLAMMER_AUF);
    akzeptiere(Token::ZEILENUMBRUCH);

    std::vector<Anweisung *> rumpf;
    if (ungleich(Token::GESCHWEIFTE_KLAMMER_ZU))
    {
        while (auto *anweisung = anweisung_einlesen())
        {
            rumpf.push_back(anweisung);
            akzeptiere(Token::ZEILENUMBRUCH);
        }
    }

    erwarte(Token::GESCHWEIFTE_KLAMMER_ZU);
    akzeptiere(Token::ZEILENUMBRUCH);

    erg = new Deklaration_Makro(
        Spanne(anfang->spanne().von(), rumpf.back()->spanne().bis()),
        name->name(), parameter, rumpf);

    return erg;
}

Asm::Deklaration *
Syntax::daten_dekl_einlesen(uint32_t größe, bool exportieren)
{
    auto *anfang = weiter();
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
        FEHLER_WENN(hex == nullptr, token(), Fehler_Wert_Erwartet);
        daten.push_back(hex);
        akzeptiere(Token::KOMMA);
    }

    auto *klammer_zu = erwarte(Token::GESCHWEIFTE_KLAMMER_ZU);

    // INFO: zeilenumbruch überspringen
    akzeptiere(Token::ZEILENUMBRUCH);

    auto erg = new Deklaration_Daten(
        Spanne(anfang->spanne().von(), klammer_zu->spanne().bis()),
        größe, name->name(), (uint16_t) daten.size(), daten, exportieren);

    return erg;
}
// }}}
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
Syntax::token(uint32_t versatz)
{
    uint32_t index = _token_index + versatz;

    if (index >= _token.size())
    {
        auto erg = _token.at(_token.size() - 1);

        return erg;
    }

    auto erg = _token.at(index);

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
        melden(token()->spanne(), Fehler_Token_Erwartet);
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
