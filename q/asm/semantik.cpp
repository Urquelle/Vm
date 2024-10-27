#include "asm/semantik.hpp"

#include "vm/anweisung.hpp"

#define Z_ANWEISUNG_OPCODE 1

namespace Asm {

Semantik::Semantik(std::vector<Ast_Knoten *> anweisungen)
    : _anweisungen(anweisungen)
{
}

std::vector<Ast_Knoten *>
Semantik::starten()
{
    markierungen_registrieren();

    for (auto anweisung : _anweisungen)
    {
        anweisung_analysieren(anweisung);
    }

    return _anweisungen;
}

void
Semantik::markierungen_registrieren()
{
    uint16_t adresse = 0;

    for (auto *knoten : _anweisungen)
    {
        if (knoten->art() == Ast_Knoten::AST_ANWEISUNG)
        {
            auto *anweisung = knoten->als<Ast_Anweisung *>();
            anweisung->adresse = adresse;

            if (anweisung->markierung())
            {
                std::string markierung = anweisung->markierung()->als<Ast_Name *>()->name();
                if (!symbol_registrieren(markierung, new Symbol_Anweisung(markierung, adresse)))
                {
                    assert(!"konnte symbol nicht registrieren");
                }
            }

            adresse += anweisung->größe();
        }

        else if (knoten->art() == Ast_Knoten::AST_SCHABLONE)
        {
            assert(!"schablone registrieren");

            auto *schablone = knoten->als<Ast_Schablone *>();

            std::map<std::string , Symbol_Schablone::Feld *> felder;
            uint16_t versatz = 0;
            for (auto *feld : schablone->felder())
            {
                auto *f = new Symbol_Schablone::Feld(versatz, feld->größe());

                if (felder.contains(feld->name()))
                {
                    assert(!"feld bereits vorhanden");
                }

                felder[feld->name()] = f;

                versatz += feld->größe();
            }

            if (!symbol_registrieren(schablone->name(), new Symbol_Schablone(schablone->name(), felder)))
            {
                assert(!"konnte symbol nicht registrieren");
            }
        }

        else if (knoten->art() == Ast_Knoten::AST_DATEN)
        {
            auto *daten = knoten->als<Ast_Daten *>();
            daten->adresse = adresse;
            std::string name = daten->name()->als<Ast_Name *>()->name();

            if (!symbol_registrieren(name, new Symbol_Daten(name, adresse)))
            {
                assert(!"konnte symbol nicht registrieren");
            }

            adresse += daten->größe();
        }

        else if (knoten->art() == Ast_Knoten::AST_KONSTANTE)
        {
            auto *konstante = knoten->als<Ast_Konstante *>();

            uint16_t wert = 0;
            if (konstante->wert()->art() == Ast_Knoten::AST_HEX)
            {
                wert = konstante->wert()->als<Ast_Hex *>()->wert();
            }
            else
            {
                assert(konstante->wert()->art() == Ast_Knoten::AST_GANZZAHL);
                wert = konstante->wert()->als<Ast_Ganzzahl *>()->wert();
            }

            std::string name = konstante->name()->als<Ast_Name *>()->name();
            if (!symbol_registrieren(name, new Symbol_Konstante(name, wert)))
            {
                assert(!"konnte symbol nicht registrieren");
            }
        }
    }
}

void
Semantik::anweisung_analysieren(Ast_Knoten *anweisung)
{
    assert(anweisung);

    if (anweisung->art() == Ast_Knoten::AST_ANWEISUNG)
    {
        auto anw = anweisung->als<Ast_Anweisung *>();
        if (anw->op()->ungleich(Ast_Knoten::AST_NAME))
        {
            assert(!"anweisung muss mit einem gültigen namen anfangen");
        }

        auto op = anw->op()->als<Ast_Name *>();

        if (strcmp(op->name(), "mov") == 0 || strcmp(op->name(), "MOV") == 0)
        {
            mov_analysieren(anw);
        }
        else if (strcmp(op->name(), "add") == 0 || strcmp(op->name(), "ADD") == 0)
        {
            add_analysieren(anw);
        }
        else if (strcmp(op->name(), "dec") == 0 || strcmp(op->name(), "DEC") == 0)
        {
            dec_analysieren(anw);
        }
        else if (strcmp(op->name(), "inc") == 0 || strcmp(op->name(), "INC") == 0)
        {
            inc_analysieren(anw);
        }
        else if (strcmp(op->name(), "jne") == 0 || strcmp(op->name(), "JNE") == 0)
        {
            jne_analysieren(anw);
        }
        else if (strcmp(op->name(), "hlt") == 0 || strcmp(op->name(), "HLT") == 0)
        {
            hlt_analysieren(anw);
        }
        else
        {
            assert(!"unbekannte anweisung");
        }
    }
}

bool
Semantik::symbol_registrieren(std::string name, Symbol *symbol)
{
    if (_symbole.contains(name))
    {
        return false;
    }

    _symbole[name] = symbol;

    return true;
}

Symbol *
Semantik::symbol_holen(std::string name)
{
    auto *erg = _symbole[name];

    if (erg == nullptr)
    {
        assert(!"konnte symbol nicht finden");
    }

    return erg;
}

void
Semantik::add_analysieren(Ast_Anweisung *add)
{
    using namespace Vm;

    auto operanden = add->operanden();
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
        add->anweisung_setzen(Anweisung::Add(op1, op2));
        add->anweisung()->_adresse = add->adresse;
    }
}

void
Semantik::mov_analysieren(Ast_Anweisung *mov)
{
    using namespace Vm;

    auto operanden = mov->operanden();
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
            mov->anweisung_setzen(Anweisung::Mov(op1, op2));
            mov->anweisung()->_adresse = mov->adresse;
        }
    }
}

void
Semantik::dec_analysieren(Ast_Anweisung *anw)
{
    using namespace Vm;

    auto operanden = anw->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anw->anweisung_setzen(Anweisung::Dec(op));
    anw->anweisung()->_adresse = anw->adresse;
}

void
Semantik::inc_analysieren(Ast_Anweisung *anw)
{
    using namespace Vm;

    auto operanden = anw->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anw->anweisung_setzen(Anweisung::Inc(op));
    anw->anweisung()->_adresse = anw->adresse;
}

void
Semantik::jne_analysieren(Ast_Anweisung *anw)
{
    using namespace Vm;

    auto operanden = anw->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 2)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);
    auto *ziel = operand_analysieren(operanden[1]);

    anw->anweisung_setzen(Anweisung::Jne(op, ziel));
    anw->anweisung()->_adresse = anw->adresse;
}

void
Semantik::hlt_analysieren(Ast_Anweisung *anw)
{
    using namespace Vm;

    anw->anweisung_setzen(Anweisung::Hlt());
    anw->anweisung()->_adresse = anw->adresse;
}

Vm::Operand *
Semantik::operand_analysieren(Ast_Knoten *op)
{
    using namespace Vm;

    if (op->art() == Ast_Knoten::AST_REG)
    {
        return Operand::Reg(op->als<Ast_Reg *>()->reg());
    }

    else if (op->art() == Ast_Knoten::AST_GANZZAHL)
    {
        auto wert = op->als<Ast_Ganzzahl *>()->wert();

        return Operand::Lit(wert);
    }

    else if (op->art() == Ast_Knoten::AST_HEX)
    {
        auto wert = op->als<Ast_Hex *>()->wert();

        return Operand::Lit(wert);
    }

    else if (op->art() == Ast_Knoten::AST_ECKIGE_KLAMMER)
    {
        uint16_t wert = ausdruck_auswerten(op->als<Ast_Eckige_Klammer *>()->ausdruck());

        return Operand::Lit(wert);
    }

    else if (op->art() == Ast_Knoten::AST_ADRESSE)
    {
        auto adr = op->als<Ast_Adresse *>();
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

    else
    {
        assert(!"unbekannter operand");
    }

    return nullptr;
}

uint16_t
Semantik::ausdruck_auswerten(Ast_Knoten *ausdruck)
{
    uint16_t erg = 0;

    switch (ausdruck->art())
    {
        case Ast_Knoten::AST_VARIABLE:
        {
            auto *var = ausdruck->als<Ast_Variable *>();
            auto *sym = symbol_holen(var->name());
            assert(sym != nullptr);

            if (sym->art() == Symbol::Konstante)
            {
                erg = sym->als<Symbol_Konstante *>()->wert();
            }
            else if (sym->art() == Symbol::Daten)
            {
                erg = sym->als<Symbol_Daten *>()->adresse();
            }
            else if (sym->art() == Symbol::Anweisung)
            {
                erg = sym->als<Symbol_Anweisung *>()->adresse();
            }
            else
            {
                assert(!"unbekannte symbolart");
            }
        } break;

        case Ast_Knoten::AST_HEX:
        {
            auto *hex = ausdruck->als<Ast_Hex *>();
            auto wert = hex->wert();

            erg = wert;
        } break;

        case Ast_Knoten::AST_GANZZAHL:
        {
            auto *gz = ausdruck->als<Ast_Ganzzahl *>();
            auto wert = gz->wert();

            erg = wert;
        } break;

        case Ast_Knoten::AST_ALS:
        {
            //
        } break;

        case Ast_Knoten::AST_BIN:
        {
            auto *bin = ausdruck->als<Ast_Bin *>();

            auto wert_links  = ausdruck_auswerten(bin->links());
            auto wert_rechts = ausdruck_auswerten(bin->rechts());

            switch (bin->op()->art())
            {
                case Token::T_PLUS:
                {
                    erg = wert_links + wert_rechts;
                } break;

                case Token::T_MINUS:
                {
                    erg = wert_links - wert_rechts;
                } break;

                case Token::T_STERN:
                {
                    erg = wert_links * wert_rechts;
                } break;

                case Token::T_PISA:
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

        case Ast_Knoten::AST_KLAMMER:
        {
            auto *klammer = ausdruck->als<Ast_Klammer *>();
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
