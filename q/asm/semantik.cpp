#include "asm/semantik.hpp"

#include "vm/anweisung.hpp"

#define Z_ANWEISUNG_OPCODE 1

namespace Asm {

Semantik::Semantik(std::vector<Ast_Knoten *> anweisungen)
    : _anweisungen(anweisungen)
    , _adresse(0)
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
                markierung_analysieren(anweisung->markierung(), adresse);
            }

            adresse += anweisung->größe();
        }

        else if (knoten->art() == Ast_Knoten::AST_DATEN)
        {
            auto *daten = knoten->als<Ast_Daten *>();
            daten->adresse = adresse;

            markierung_analysieren(daten->name(), adresse);

            adresse += daten->größe();
        }

        else if (knoten->art() == Ast_Knoten::AST_KONSTANTE)
        {
            auto *konstante = knoten->als<Ast_Konstante *>();

            markierung_analysieren(konstante->name(), adresse);
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
    else if (anweisung->gleich(Ast_Knoten::AST_DATEN))
    {
#if 0
        auto *daten = anweisung->als<Ast_Daten *>();
        daten->adresse = _adresse;

        markierung_analysieren(daten->name(), _adresse);

        auto z_daten = daten->z_daten();
        auto anz_daten = daten->anz_daten();

        _adresse += (z_daten * anz_daten);
#endif
    }
    else if (anweisung->gleich(Ast_Knoten::AST_KONSTANTE))
    {
#if 0
        auto konst = anweisung->als<Ast_Konstante *>();

        assert(konst->wert()->gleich(Ast_Knoten::AST_HEX) || konst->wert()->gleich(Ast_Knoten::AST_GANZZAHL));
        auto knoten = konst->wert();
        uint16_t wert = 0;

        if (knoten->gleich(Ast_Knoten::AST_HEX))
        {
            wert = knoten->als<Ast_Hex *>()->wert();
        }
        else if (knoten->gleich(Ast_Knoten::AST_GANZZAHL))
        {
            wert = knoten->als<Ast_Ganzzahl *>()->wert();
        }

        markierung_analysieren(konst->name(), wert);
#endif
    }
}

void
Semantik::markierung_analysieren(Ast_Knoten *name, uint16_t wert)
{
    if (name->ungleich(Ast_Knoten::AST_NAME))
    {
        assert(!"namen erwartet");
    }

    auto markierung = name->als<Ast_Name *>();

    if (_markierungen.contains(markierung->name()))
    {
        assert(!"markierung existiert bereits");
    }

    _markierungen[markierung->name()] = wert;

    for (auto *op : _abhängigkeiten[markierung->name()])
    {
        op->als<Vm::Operand_Adr *>()->setzen(wert);
    }
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
        return Operand::Reg(op->als<Ast_Ganzzahl *>()->wert());
    }

    else if (op->art() == Ast_Knoten::AST_KONSTANTE)
    {
        auto konst = op->als<Ast_Konstante *>();

        auto name = konst->name()->als<Ast_Name *>()->name();
        auto wert = _markierungen[name];

        return Operand::Lit(wert);
    }

    else if (op->art() == Ast_Knoten::AST_HEX)
    {
        auto wert = op->als<Ast_Hex *>()->wert();

        return Operand::Lit(wert);
    }

    else if (op->art() == Ast_Knoten::AST_ECKIGE_KLAMMER)
    {
        auto *erg = operand_ausdruck_analysieren(op->als<Ast_Eckige_Klammer *>()->ausdruck());
    }

    else if (op->art() == Ast_Knoten::AST_VARIABLE)
    {
        auto name = op->als<Ast_Variable *>()->name();
        auto *erg = Operand::Adr(0);

        if (_markierungen.contains(name))
        {
            auto wert = _markierungen[name];
            erg->als<Operand_Adr *>()->setzen(wert);
        }
        else
        {
            // AUFGABE: abhängigkeit eintragen
            _abhängigkeiten[name].push_back(erg);
        }

        return erg;
    }

    else if (op->art() == Ast_Knoten::AST_ADRESSE)
    {
        auto adr = op->als<Ast_Adresse *>();
        auto aus = operand_ausdruck_analysieren(adr->ausdruck());

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

    return nullptr;
}

Vm::Operand *
Semantik::operand_ausdruck_analysieren(Ast_Knoten *ausdruck)
{
    assert(ausdruck);

    switch (ausdruck->art())
    {
        case Ast_Knoten::AST_REG:
        {
            return operand_analysieren(ausdruck);
        } break;

        case Ast_Knoten::AST_GANZZAHL:
        {
            return Vm::Operand::Lit(ausdruck->als<Ast_Ganzzahl *>()->wert());
        } break;

        case Ast_Knoten::AST_HEX:
        {
            return Vm::Operand::Lit(ausdruck->als<Ast_Hex *>()->wert());
        } break;

        case Ast_Knoten::AST_VARIABLE:
        {
            auto *variable = ausdruck->als<Ast_Variable *>();

            if (!_markierungen.contains(variable->name()))
            {
                assert(!"variable existiert nicht");
            }

            auto wert = _markierungen[variable->name()];

            return Vm::Operand::Lit(wert);
        } break;

        case Ast_Knoten::AST_BIN:
        {
            auto *op_links  = operand_ausdruck_analysieren(ausdruck->als<Ast_Bin *>()->links());
            auto *op_rechts = operand_ausdruck_analysieren(ausdruck->als<Ast_Bin *>()->rechts());

            auto wert = 0;

            return Vm::Operand::Lit(wert);
        } break;

        case Ast_Knoten::AST_ECKIGE_KLAMMER:
        {
            auto innere_ausdruck = ausdruck->als<Ast_Eckige_Klammer *>()->ausdruck();

            return operand_ausdruck_analysieren(innere_ausdruck);
        } break;

        default:
        {
            assert(!"unbekannter ausdruck.");
            return nullptr;
        } break;
    }
}

}
