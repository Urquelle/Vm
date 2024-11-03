#include "asm/semantik.hpp"

#include "vm/anweisung.hpp"

#define FEHLER_WENN(A, F) do { if (A) { melden(token()->spanne(), F); } } while(0)

namespace Asm {

Semantik::Semantik(Ast ast)
    : _ast(ast)
    , _zone(new Zone("Welt", nullptr))
{
}

Ast
Semantik::starten()
{
    makros_registrieren();
    makros_erweitern();
    markierungen_registrieren();

    for (auto anweisung : _ast.anweisungen)
    {
        anweisung_analysieren(anweisung);
    }

    return _ast;
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

        auto *vorhandenes_symbol = symbol_holen(makro_dekl->name());
        if (vorhandenes_symbol != nullptr)
        {
            melden(makro_dekl->spanne(), new Fehler(std::format("makro '{}' bereits vorhanden", makro_dekl->name())));
        }

        if (vorhandenes_symbol == nullptr)
        {
            auto *sym = new Symbol_Makro(makro_dekl->name(), makro_dekl);
            sym->zone_setzen(new Zone(makro_dekl->name(), _zone));

            if (!symbol_registrieren(makro_dekl->name(), sym))
            {
                melden(makro_dekl->spanne(), new Fehler(std::format("makro '{}' konnte nicht registriert werden.", makro_dekl->name())));
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
            }

            auto *sym_makro = sym->als<Symbol_Makro *>();

            // INFO: anzahl der argumente, muss gleich der anzahl der parameter sein.
            if (anweisung->argumente().size() != sym_makro->parameter().size())
            {
                melden(anweisung, new Fehler(
                    std::format("anzahl der argumente ({}) entspricht nicht der anzahl der parameter ({})",
                                anweisung->argumente().size(), sym_makro->parameter().size())));
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
    uint16_t adresse = 0;

    for (auto *dekl : _ast.deklarationen)
    {
        if (dekl->art() == Deklaration::SCHABLONE)
        {
            auto *schablone = dekl->als<Deklaration_Schablone *>();

            std::map<std::string , Symbol_Schablone::Feld *> felder;
            uint16_t versatz = 0;
            for (auto *feld : schablone->felder())
            {
                if (felder.contains(feld->name()))
                {
                    melden(feld->spanne(), new Fehler(std::format("feld '{}' bereits vorhanden", feld->name())));
                }

                auto *f = new Symbol_Schablone::Feld(versatz, feld->größe());

                felder[feld->name()] = f;
                versatz += feld->größe();
            }

            if (!symbol_registrieren(schablone->name(), new Symbol_Schablone(schablone->name(), felder)))
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
            // INFO: makros wurden bereits in makros_registrieren behandelt
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
            assert(!"nicht implementiert");

            // AUFGABE: a) als allererstes die makros durchgehen
            //          und als symbol registrieren. dann alle anweisungen durchgehen, die makros aufrufen und diese durch
            //          den makrorumpf ersetzen?
            //          b) makros auflösen und ast analysieren, und in einer zusätzlichen phase die adressen berechnen
        }

        else if (a->art() == Anweisung::MARKIERUNG)
        {
            auto *markierung = a->als<Anweisung_Markierung *>();
            markierung->adresse_setzen(adresse);
            if (!symbol_registrieren(markierung->name(), new Symbol_Markierung(markierung->name(), adresse)))
            {
                //
            }
        }
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
                    Symbol *sym = symbol_holen(operand->als<Name *>()->name());
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

    if (op1->gleich(Operand::OPND_REG) && op2->gleich(Operand::OPND_REG))
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
        if (op1->gleich(Operand::OPND_REG) && op2->gleich(Operand::OPND_LIT))
        {
            erfolg = true;
        }

        // INFO: mov reg reg
        else if (op1->gleich(Operand::OPND_REG) && op2->gleich(Operand::OPND_REG))
        {
            erfolg = true;
        }

        // INFO: mov reg adr
        else if (op1->gleich(Operand::OPND_REG) && op2->gleich(Operand::OPND_ADR))
        {
            erfolg = true;
        }

        // INFO: mov adr reg
        else if (op1->gleich(Operand::OPND_ADR) && op2->gleich(Operand::OPND_REG))
        {
            erfolg = true;
        }

        // INFO: mov adr lit
        else if (op1->gleich(Operand::OPND_ADR) && op2->gleich(Operand::OPND_LIT))
        {
            erfolg = true;
        }

        // INFO: mov reg zgr
        else if (op1->gleich(Operand::OPND_REG) && op2->gleich(Operand::OPND_ZGR))
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
        return Operand::Reg(op->als<Reg *>()->reg());
    }

    else if (op->art() == Ausdruck::HEX)
    {
        auto wert = op->als<Hex *>()->wert();

        return Operand::Lit(wert);
    }

    else if (op->art() == Ausdruck::AUSWERTUNG)
    {
        uint16_t wert = ausdruck_auswerten(op->als<Auswertung *>()->ausdruck());

        return Operand::Lit(wert);
    }

    else if (op->art() == Ausdruck::ADRESSE)
    {
        auto adr = op->als<Adresse *>();
        auto aus = operand_analysieren(adr->ausdruck());

        if (aus->art() == Operand::OPND_REG)
        {
            return Operand::Zgr(aus);
        }
        else if (aus->art() == Operand::OPND_LIT)
        {
            return Operand::Adr(aus->als<Operand_Lit *>()->wert());
        }
        else
        {
            assert(!"unbekannter operand");
        }
    }

    else if (op->art() == Ausdruck::NAME)
    {
        auto *name = op->als<Name *>();
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

uint16_t
Semantik::ausdruck_auswerten(Ausdruck *ausdruck)
{
    uint16_t erg = 0;

    switch (ausdruck->art())
    {
        case Ausdruck::VARIABLE:
        {
            auto *var = ausdruck->als<Variable *>();
            auto *sym = symbol_holen(var->name());
            assert(sym != nullptr);

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
            auto *hex = ausdruck->als<Hex *>();
            auto wert = hex->wert();

            erg = wert;
        } break;

        case Ausdruck::ALS:
        {
            auto *als = ausdruck->als<Als *>();

            auto *sym = symbol_holen(als->schablone());
            if (sym == nullptr)
            {
                assert(!"keine passende schablone gefunden");
            }

            auto *basis = symbol_holen(als->basis());
            if (basis == nullptr)
            {
                assert(!"keine passende datenvariable gefunden");
            }

            if (basis->art() != Symbol::DATEN)
            {
                assert(!"basis muss vom datentyp data8 data16 sein");
            }

            auto *schablone = sym->als<Symbol_Schablone *>();
            auto *feld = schablone->felder()[als->feld()];

            if (feld == nullptr)
            {
                assert(!"das gesuchte feld existiert nicht in der schablone");
            }

            erg = basis->als<Symbol_Daten *>()->adresse() + feld->versatz();
        } break;

        case Ausdruck::BIN:
        {
            auto *bin = ausdruck->als<Bin *>();

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
            auto *klammer = ausdruck->als<Klammer *>();
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
