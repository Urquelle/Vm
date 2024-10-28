#include "asm/semantik.hpp"

#include "vm/anweisung.hpp"

#define Z_ANWEISUNG_OPCODE 1

namespace Asm {

Semantik::Semantik(Ast ast)
    : _ast(ast)
{
}

Ast
Semantik::starten()
{
    markierungen_registrieren();

    for (auto anweisung : _ast.anweisungen)
    {
        anweisung_analysieren(anweisung);
    }

    return _ast;
}

void
Semantik::markierungen_registrieren()
{
    uint16_t adresse = 0;

    for (auto *dekl : _ast.deklarationen)
    {
        if (dekl->art() == Deklaration::SCHABLONE)
        {
            auto *schablone = dekl->als<Schablone *>();

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

        else if (dekl->art() == Deklaration::DATEN)
        {
            auto *daten = dekl->als<Daten *>();
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
            auto *konstante = dekl->als<Konstante *>();

            uint16_t wert = konstante->wert();
            std::string name = konstante->name();

            if (!symbol_registrieren(name, new Symbol_Konstante(name, wert)))
            {
                assert(!"konnte symbol nicht registrieren");
            }
        }
    }

    for (auto *anweisung : _ast.anweisungen)
    {
        anweisung->adresse = adresse;

        if (anweisung->markierung())
        {
            std::string markierung = anweisung->markierung()->als<Name *>()->name();
            if (!symbol_registrieren(markierung, new Symbol_Anweisung(markierung, adresse)))
            {
                assert(!"konnte symbol nicht registrieren");
            }
        }

        adresse += anweisung->größe();
    }
}

void
Semantik::anweisung_analysieren(Asm::Anweisung *anweisung)
{
    assert(anweisung);

    if (anweisung->op() == "mov" || anweisung->op() == "MOV")
    {
        mov_analysieren(anweisung);
    }
    else if (anweisung->op() == "add" || anweisung->op() == "ADD")
    {
        add_analysieren(anweisung);
    }
    else if (anweisung->op() == "dec" || anweisung->op() == "DEC")
    {
        dec_analysieren(anweisung);
    }
    else if (anweisung->op() == "inc" || anweisung->op() == "INC")
    {
        inc_analysieren(anweisung);
    }
    else if (anweisung->op() == "jne" || anweisung->op() == "JNE")
    {
        jne_analysieren(anweisung);
    }
    else if (anweisung->op() == "hlt" || anweisung->op() == "HLT")
    {
        hlt_analysieren(anweisung);
    }
    else if (anweisung->op() == "rti" || anweisung->op() == "RTI")
    {
        rti_analysieren(anweisung);
    }
    else
    {
        assert(!"unbekannte anweisung");
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
Semantik::add_analysieren(Asm::Anweisung *anweisung)
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
        anweisung->anweisung_setzen(Vm::Anweisung::Add(op1, op2));
    }
}

void
Semantik::mov_analysieren(Asm::Anweisung *anweisung)
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
            anweisung->anweisung_setzen(Vm::Anweisung::Mov(op1, op2));
        }
    }
}

void
Semantik::dec_analysieren(Asm::Anweisung *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anweisung->anweisung_setzen(Vm::Anweisung::Dec(op));
}

void
Semantik::inc_analysieren(Asm::Anweisung *anweisung)
{
    using namespace Vm;

    auto operanden = anweisung->operanden();
    auto anzahl_operanden = operanden.size();

    if (anzahl_operanden != 1)
    {
        assert(!"falsche anzahl der operanden");
    }

    auto *op = operand_analysieren(operanden[0]);

    anweisung->anweisung_setzen(Vm::Anweisung::Inc(op));
}

void
Semantik::jne_analysieren(Asm::Anweisung *anweisung)
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

    anweisung->anweisung_setzen(Vm::Anweisung::Jne(op, ziel));
}

void
Semantik::hlt_analysieren(Asm::Anweisung *anweisung)
{
    using namespace Vm;

    anweisung->anweisung_setzen(Vm::Anweisung::Hlt());
}

void
Semantik::rti_analysieren(Asm::Anweisung *anweisung)
{
    using namespace Vm;

    anweisung->anweisung_setzen(Vm::Anweisung::Rti());
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
