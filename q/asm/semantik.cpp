#include "asm/semantik.hpp"
#include "vm/anweisung.hpp"

#include <cassert>
#include <format>

namespace Asm {

Semantik::Semantik(Ast ast, Zone *zone)
    : _ast(ast)
    , _start_adresse(0)
    , _zone(zone)
{
}

Diagnostik
Semantik::diagnostik()
{
    return _diagnostik;
}

void
Semantik::melden(Spanne spanne, Fehler *fehler)
{
    _diagnostik.melden(spanne, fehler);
}

void
Semantik::melden(Anweisung *anweisung, Fehler *fehler)
{
    melden(anweisung->spanne(), fehler);
}

void
Semantik::melden(Ausdruck *ausdruck, Fehler *fehler)
{
    melden(ausdruck->spanne(), fehler);
}

void
Semantik::melden(Token *token, Fehler *fehler)
{
    melden(token->spanne(), fehler);
}

void
Semantik::melden(Modul *modul, Fehler *fehler)
{
    melden(modul->spanne(), fehler);
}

Zone *
Semantik::zone()
{
    return _zone;
}

Zone *
Semantik::zone_betreten(Zone *z)
{
    z->über_setzen(zone());
    _zone = z;

    return _zone->über();
}

void
Semantik::zone_verlassen()
{
    _zone = _zone->über();
}

Ast
Semantik::starten(uint16_t start_adresse)
{
    _start_adresse = start_adresse;

    module_importieren();
    makros_registrieren();
    makros_erweitern();
    markierungen_registrieren();
    anweisungen_analysieren();

    return _ast;
}

void
Semantik::module_importieren()
{
    for (auto *modul : _ast.module)
    {
        Symbol *sym = new Symbol_Modul(modul->name(), modul);
        sym->zone_setzen(new Zone(modul->name()));

        if (!symbol_registrieren(modul->name(), sym))
        {
            melden(modul, new Fehler(std::format("modul {} konnte nicht importiert werden", modul->name())));
        }

        Semantik semantik = Semantik({
            .deklarationen = modul->deklarationen(),
            .anweisungen = modul->anweisungen()
        });

        auto ast = semantik.starten(modul->adresse());

        for (auto *deklaration : ast.deklarationen)
        {
            if (deklaration->exportieren())
            {
                sym->zone()->registrieren(
                    deklaration->name(),
                    semantik.zone()->symbol(deklaration->name()));
            }
        }

        for (auto *anweisung : ast.anweisungen)
        {
            _ast.anweisungen.push_back(anweisung);
        }
    }
}

void
Semantik::makros_registrieren()
{
    for (auto *dekl : _ast.deklarationen)
    {
        if (dekl->art() != Deklaration::MAKRO)
        {
            continue;
        }

        auto *makro_dekl = dekl->als<Deklaration_Makro *>();

        if (symbol_holen(makro_dekl->name()) != nullptr)
        {
            melden(makro_dekl->spanne(), new Fehler(std::format("makro '{}' bereits vorhanden", makro_dekl->name())));

            return;
        }

        auto *sym = new Symbol_Makro(makro_dekl->name(), makro_dekl);
        sym->zone_setzen(new Zone(makro_dekl->name(), _zone));

        if (!symbol_registrieren(makro_dekl->name(), sym))
        {
            melden(makro_dekl->spanne(), new Fehler(std::format("makro '{}' konnte nicht registriert werden.", makro_dekl->name())));

            return;
        }

        // INFO: makro parameter in zone registrieren
        for (auto *param : makro_dekl->parameter())
        {
            if (sym->zone()->registriert(param->name()))
            {
                melden(param, new Fehler(std::format("das makro {} enthält bereits einen parameter {}",
                    makro_dekl->name(), param->name())));
            }

            auto *param_sym = new Symbol_Platzhalter(param->name());
            if (!sym->zone()->registrieren(param->name(), param_sym))
            {
                melden(param, new Fehler("konnte symbol nicht registrieren"));
            }
        }
    }
}

void
Semantik::makros_erweitern()
{
    std::list<Anweisung *> anweisungen;

    for (auto *a : _ast.anweisungen)
    {
        if (a->art() == Anweisung::MAKRO)
        {
            auto *anweisung = a->als<Anweisung_Makro *>();
            auto *sym = symbol_holen(anweisung->name());

            if (sym == nullptr)
            {
                melden(a, new Fehler(std::format("unbekanntes makro {}", anweisung->name())));

                continue;
            }

            auto *sym_makro = sym->als<Symbol_Makro *>();

            // INFO: anzahl der argumente, muss gleich der anzahl der parameter sein.
            if (anweisung->argumente().size() != sym_makro->parameter().size())
            {
                melden(anweisung, new Fehler(
                    std::format("anzahl der argumente ({}) entspricht nicht der anzahl der parameter ({})",
                                anweisung->argumente().size(), sym_makro->parameter().size())));

                continue;
            }

            // INFO: argumente an die entsprechenden parameter des makros binden, und
            //       die makroangaben auflösen
            for (int32_t i = 0; i < sym_makro->parameter().size(); ++i)
            {
                auto *param = sym_makro->parameter()[i];
                auto *sym_param = sym_makro->zone()->symbol(param->name());

                sym_param->als<Symbol_Platzhalter *>()->ausdruck_setzen(anweisung->argumente()[i]);
            }

            zone_betreten(sym_makro->zone());

            for (auto *makro_anweisung : sym_makro->dekl()->rumpf())
            {
                auto *kopie = anweisung_kopieren(makro_anweisung);
                anweisungen.push_back(anweisung_analysieren(kopie));
            }

            zone_verlassen();
        }
        else
        {
            anweisungen.push_back(a);
        }
    }

    _ast.anweisungen = anweisungen;
}

void
Semantik::markierungen_registrieren()
{
    uint16_t adresse = _start_adresse;

    for (auto *dekl : _ast.deklarationen)
    {
        if (dekl->art() == Deklaration::SCHABLONE)
        {
            auto *schablone = dekl->als<Deklaration_Schablone *>();
            Zone *felder = new Zone(schablone->name());

            uint16_t versatz = 0;
            for (auto *feld : schablone->felder())
            {
                if (felder->registriert(feld->name()))
                {
                    melden(feld->spanne(), new Fehler(std::format("feld '{}' bereits vorhanden", feld->name())));
                }

                felder->registrieren(feld->name(), new Symbol_Feld(feld->name(), versatz, feld->größe()));
                versatz += feld->größe();
            }

            auto *sym = new Symbol_Schablone(schablone->name(), felder);
            if (!symbol_registrieren(schablone->name(), sym))
            {
                melden(schablone->spanne(), new Fehler(std::format("Schablone '{}' konnte nicht registriert werden", schablone->name())));
            }
        }

        else if (dekl->art() == Deklaration::DATEN)
        {
            auto *daten = dekl->als<Deklaration_Daten *>();
            daten->adresse = adresse;
            std::string name = daten->name();

            if (!symbol_registrieren(name, new Symbol_Daten(name, adresse)))
            {
                assert(!"konnte symbol nicht registrieren");
            }

            adresse += daten->gesamtgröße();
        }

        else if (dekl->art() == Deklaration::KONSTANTE)
        {
            auto *konstante = dekl->als<Deklaration_Konstante *>();

            uint16_t wert = konstante->wert();
            std::string name = konstante->name();

            if (!symbol_registrieren(name, new Symbol_Konstante(name, wert)))
            {
                assert(!"konnte symbol nicht registrieren");
            }
        }

        else if (dekl->art() == Deklaration::MAKRO)
        {
            // INFO: makro deklarationen wurden bereits in makros_registrieren behandelt
        }
    }

    for (auto *a : _ast.anweisungen)
    {
        if (a->art() == Anweisung::ASM)
        {
            auto *anweisung = a->als<Anweisung_Asm *>();
            anweisung->adresse_setzen(adresse);
            adresse += anweisung->größe();
        }

        else if (a->art() == Anweisung::MAKRO)
        {
            // INFO: makro anweisungen wurden bereits in makros_erweitern() behandelt
        }

        else if (a->art() == Anweisung::MARKIERUNG)
        {
            auto *markierung = a->als<Anweisung_Markierung *>();
            markierung->adresse_setzen(adresse);

            if (!symbol_registrieren(markierung->name(), new Symbol_Markierung(markierung->name(), adresse)))
            {
                melden(markierung, new Fehler(std::format("markierung {} konnte nicht registriert werden", markierung->name())));
            }
        }
    }
}

void
Semantik::anweisungen_analysieren()
{
    for (auto anweisung : _ast.anweisungen)
    {
        anweisung_analysieren(anweisung);
    }
}

Anweisung *
Semantik::anweisung_analysieren(Asm::Anweisung *a)
{
    assert(a);

    if (a->art() == Anweisung::ASM)
    {
        auto *anweisung = a->als<Anweisung_Asm *>();

        if (anweisung->op() == "mov" || anweisung->op() == "MOV")
        {
            return mov_analysieren(anweisung);
        }
        else if (anweisung->op() == "add" || anweisung->op() == "ADD")
        {
            return add_analysieren(anweisung);
        }
        else if (anweisung->op() == "dec" || anweisung->op() == "DEC")
        {
            return dec_analysieren(anweisung);
        }
        else if (anweisung->op() == "inc" || anweisung->op() == "INC")
        {
            return inc_analysieren(anweisung);
        }
        else if (anweisung->op() == "jne" || anweisung->op() == "JNE")
        {
            return jne_analysieren(anweisung);
        }
        else if (anweisung->op() == "hlt" || anweisung->op() == "HLT")
        {
            return hlt_analysieren(anweisung);
        }
        else if (anweisung->op() == "rti" || anweisung->op() == "RTI")
        {
            return rti_analysieren(anweisung);
        }
        else if (anweisung->op() == "brk" || anweisung->op() == "BRK")
        {
            return brk_analysieren(anweisung);
        }
        else
        {
            assert(!"unbekannte anweisung");
            return nullptr;
        }
    }
    else if (a->art() == Anweisung::MARKIERUNG)
    {
        // INFO: markierungen wurden bereits in markierungen_registrieren behandelt
    }
    else if (a->art() == Anweisung::MAKRO)
    {
        // INFO: makro anweisungen wurde bereits in makros_erweitern behandelt
    }
    else
    {
        assert(!"wie sind wir hergekommen?");
    }

    return nullptr;
}

Anweisung *
Semantik::anweisung_kopieren(Anweisung *anweisung)
{
    switch (anweisung->art())
    {
        case Anweisung::ASM:
        {
            auto *a = anweisung->als<Anweisung_Asm *>();

            std::vector<Ausdruck *> operanden;
            for (int32_t i = 0; i < a->operanden().size(); ++i)
            {
                auto *operand = a->operanden()[i];

                if (operand->art() == Ausdruck::NAME)
                {
                    Symbol *sym = symbol_holen(operand->als<Ausdruck_Name *>()->name());
                    assert(sym->art() == Symbol::PLATZHALTER);
                    operanden.push_back(sym->als<Symbol_Platzhalter *>()->ausdruck());
                }
                else
                {
                    operanden.push_back(operand);
                }
            }

            return new Anweisung_Asm(a->spanne(), a->op(), operanden);
        } break;

        default: return anweisung;
    }
}

bool
Semantik::symbol_registrieren(std::string name, Symbol *symbol)
{
    auto erg = _zone->registrieren(name, symbol);

    return erg;
}

bool
Semantik::symbol_registriert(std::string name)
{
    bool erg = _zone->registriert(name);

    return erg;
}

Symbol *
Semantik::symbol_holen(std::string name)
{
    auto erg = _zone->suchen(name);

    if (erg.schlecht())
    {
        return nullptr;
    }

    return erg.wert();
}

Anweisung *
Semantik::add_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden < 2 || anzahl_operanden > 3)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto op1 = operand_analysieren(operanden[0]);
    auto op2 = operand_analysieren(operanden[1]);

    auto erfolg = false;

    if (op1->gleich(Vm::Operand::OPND_REG) && op2->gleich(Vm::Operand::OPND_REG))
    {
        erfolg = true;
    }
    else
    {
        assert(!"andere varianten implementieren");
    }

    if (erfolg)
    {
        anweisung->vm_anweisung_setzen(Vm::Anweisung::Add(op1, op2));

        return anweisung;
    }

    return nullptr;
}

Anweisung *
Semantik::mov_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden < 2 || anzahl_operanden > 3)
    {
        assert(!"falsche anzahl der operanden");
    }

    if (anzahl_operanden == 2)
    {
        auto op1 = operand_analysieren(operanden[0]);
        auto op2 = operand_analysieren(operanden[1]);

        auto erfolg = false;

        // INFO: mov reg lit
        if (op1->gleich(Vm::Operand::OPND_REG) && op2->gleich(Vm::Operand::OPND_LIT))
        {
            erfolg = true;
        }

        // INFO: mov reg reg
        else if (op1->gleich(Vm::Operand::OPND_REG) && op2->gleich(Vm::Operand::OPND_REG))
        {
            erfolg = true;
        }

        // INFO: mov reg adr
        else if (op1->gleich(Vm::Operand::OPND_REG) && op2->gleich(Vm::Operand::OPND_ADR))
        {
            erfolg = true;
        }

        // INFO: mov adr reg
        else if (op1->gleich(Vm::Operand::OPND_ADR) && op2->gleich(Vm::Operand::OPND_REG))
        {
            erfolg = true;
        }

        // INFO: mov adr lit
        else if (op1->gleich(Vm::Operand::OPND_ADR) && op2->gleich(Vm::Operand::OPND_LIT))
        {
            erfolg = true;
        }

        // INFO: mov reg zgr
        else if (op1->gleich(Vm::Operand::OPND_REG) && op2->gleich(Vm::Operand::OPND_ZGR))
        {
            erfolg = true;
        }

        if (erfolg)
        {
            anweisung->vm_anweisung_setzen(Vm::Anweisung::Mov(op1, op2));
        }

        return anweisung;
    }
    else if (anzahl_operanden == 3)
    {
        assert(!"3 operanden für mov");
    }

    return nullptr;
}

Anweisung *
Semantik::dec_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Dec(op));

    return anweisung;
}

Anweisung *
Semantik::inc_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Inc(op));

    return anweisung;
}

Anweisung *
Semantik::jne_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 2)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);
    auto *ziel = operand_analysieren(operanden[1]);

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Jne(op, ziel));

    return anweisung;
}

Anweisung *
Semantik::hlt_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Hlt());

    return anweisung;
}

Anweisung *
Semantik::rti_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Rti());

    return anweisung;
}

Anweisung *
Semantik::brk_analysieren(Asm::Anweisung_Asm *anweisung)
{
    using namespace Vm;

    anweisung->vm_anweisung_setzen(Vm::Anweisung::Brk());

    return anweisung;
}

Vm::Operand *
Semantik::operand_analysieren(Ausdruck *op)
{
    using namespace Vm;

    if (op->art() == Ausdruck::REG)
    {
        return Vm::Operand::Reg(op->als<Ausdruck_Reg *>()->reg());
    }

    else if (op->art() == Ausdruck::HEX)
    {
        auto wert = op->als<Ausdruck_Hex *>()->wert();

        return Vm::Operand::Lit(wert);
    }

    else if (op->art() == Ausdruck::AUSWERTUNG)
    {
        uint16_t wert = ausdruck_auswerten(op->als<Ausdruck_Auswertung *>()->ausdruck());

        return Vm::Operand::Lit(wert);
    }

    else if (op->art() == Ausdruck::ADRESSE)
    {
        auto adr = op->als<Ausdruck_Adresse *>();
        auto aus = operand_analysieren(adr->ausdruck());

        if (aus->art() == Vm::Operand::OPND_REG)
        {
            return Vm::Operand::Zgr(aus);
        }
        else if (aus->art() == Vm::Operand::OPND_LIT)
        {
            return Vm::Operand::Adr(aus->als<Operand_Lit *>()->wert());
        }
        else
        {
            assert(!"unbekannter operand");
        }
    }

    else if (op->art() == Ausdruck::NAME)
    {
        auto *name = op->als<Ausdruck_Name *>();
        auto *sym = symbol_holen(name->name());

        auto *erg = operand_analysieren(sym->als<Symbol_Platzhalter *>()->ausdruck());

        return erg;
    }

    else
    {
        assert(!"unbekannter operand");
    }

    return nullptr;
}

Ergebnis<Asm::Operand *>
Semantik::ausdruck_analysieren(Ausdruck *ausdruck)
{
    Asm::Operand *erg = nullptr;

    switch (ausdruck->art())
    {
        case Ausdruck::NAME:
        {
            auto name = ausdruck->als<Ausdruck_Name *>()->name();

            erg = new Asm::Operand(symbol_holen(name));
        } break;

        case Ausdruck::FELD:
        {
            auto *feld = ausdruck->als<Ausdruck_Feld *>();

            auto basis = ausdruck_analysieren(feld->basis());
            if (basis.schlecht())
            {
                melden(feld->basis(), basis.fehler());

                return basis.fehler();
            }

            auto symbol = basis.wert()->symbol()->zone()->suchen(feld->feld());
            if (symbol.schlecht())
            {
                melden(feld, symbol.fehler());
            }

            erg = new Operand(symbol.wert());
        } break;

        case Ausdruck::KLAMMER:
        {
            auto op = ausdruck_analysieren(ausdruck->als<Ausdruck_Klammer *>()->ausdruck());

            if (op.schlecht())
            {
                melden(ausdruck, op.fehler());
            }

            erg = op.wert();
        } break;

        case Ausdruck::VARIABLE:
        case Ausdruck::BIN:
        case Ausdruck::REG:
        case Ausdruck::HEX:
        case Ausdruck::AUSWERTUNG:
        case Ausdruck::ALS:
        case Ausdruck::TEXT:
        case Ausdruck::ADRESSE:
        case Ausdruck::BLOCK:
        default: assert(!"unbekannter ausdruck");
    }

    return erg;
}

uint16_t
Semantik::ausdruck_auswerten(Ausdruck *ausdruck)
{
    uint16_t erg = 0;

    switch (ausdruck->art())
    {
        case Ausdruck::VARIABLE:
        {
            auto *variable = ausdruck->als<Ausdruck_Variable *>();
            auto operand = ausdruck_analysieren(variable->ausdruck());

            if (operand.schlecht())
            {
                melden(variable, operand.fehler());
                return erg;
            }

            auto *sym = operand.wert()->symbol();

            if (sym->art() == Symbol::KONSTANTE)
            {
                erg = sym->als<Symbol_Konstante *>()->wert();
            }
            else if (sym->art() == Symbol::DATEN)
            {
                erg = sym->als<Symbol_Daten *>()->adresse();
            }
            else if ( sym->art() == Symbol::MARKIERUNG)
            {
                erg = sym->als<Symbol_Markierung *>()->adresse();
            }
            else if (sym->art() == Symbol::ANWEISUNG)
            {
                erg = sym->als<Symbol_Anweisung *>()->adresse();
            }
            else
            {
                assert(!"unbekannte symbolart");
            }
        } break;

        case Ausdruck::HEX:
        {
            auto *hex = ausdruck->als<Ausdruck_Hex *>();
            auto wert = hex->wert();

            erg = wert;
        } break;

        case Ausdruck::ALS:
        {
            auto *als = ausdruck->als<Ausdruck_Als *>();

            auto schablone = ausdruck_analysieren(als->schablone());
            if (schablone.schlecht())
            {
                melden(als->schablone(), schablone.fehler());
                return erg;
            }

            auto basis = ausdruck_analysieren(als->variable()->basis());
            if (basis.schlecht())
            {
                melden(als->variable()->basis(), basis.fehler());
                return erg;
            }

            auto *feld = schablone.wert()->symbol()->zone()->symbol(als->variable()->feld());
            if (feld == nullptr)
            {
                melden(als->variable(), new Fehler("feld nicht gefunden"));
                return erg;
            }

            erg +=
                basis.wert()->symbol()->als<Symbol_Daten *>()->adresse() +
                feld->als<Symbol_Feld *>()->versatz();
        } break;

        case Ausdruck::BIN:
        {
            auto *bin = ausdruck->als<Ausdruck_Bin *>();

            auto wert_links  = ausdruck_auswerten(bin->links());
            auto wert_rechts = ausdruck_auswerten(bin->rechts());

            switch (bin->op()->art())
            {
                case Token::PLUS:
                {
                    erg = wert_links + wert_rechts;
                } break;

                case Token::MINUS:
                {
                    erg = wert_links - wert_rechts;
                } break;

                case Token::STERN:
                {
                    erg = wert_links * wert_rechts;
                } break;

                case Token::PISA:
                {
                    assert(wert_rechts != 0);
                    erg = wert_links / wert_rechts;
                } break;

                default:
                {
                    assert(!"unbekannter operator.");
                } break;
            }
        } break;

        case Ausdruck::KLAMMER:
        {
            auto *klammer = ausdruck->als<Ausdruck_Klammer *>();
            erg = ausdruck_auswerten(klammer->ausdruck());
        } break;

        default:
        {
            assert(!"unzulässiger ausdruck");
        } break;
    }

    return erg;
}

}
