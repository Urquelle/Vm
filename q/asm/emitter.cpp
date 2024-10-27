#include "emitter.hpp"

#include "asm/ast.hpp"

#define ASM_STOPP()   __debugbreak()

namespace Asm {

Emitter::Emitter(Vm::Laufwerk *laufwerk, std::vector<Ast_Knoten *> ast)
    : _laufwerk(laufwerk)
    , _ast(ast)
{
}

void
Emitter::starten()
{
    for (auto *knoten : _ast)
    {
        if (knoten->art() == Ast_Knoten::AST_KONSTANTE)
        {
            continue;
        }

        knoten_emittieren(knoten);
    }
}

void
Emitter::knoten_emittieren(Ast_Knoten *knoten)
{
    switch (knoten->art())
    {
        case Ast_Knoten::AST_ANWEISUNG:
        {
            auto *ast = knoten->als<Ast_Anweisung *>();
            auto *anweisung = ast->anweisung();
            assert(anweisung != nullptr);
            anweisung->kodieren(_laufwerk);
        } break;

        case Ast_Knoten::AST_DATEN:
        {
            auto *ast = knoten->als<Ast_Daten *>();
            uint16_t adresse = ast->adresse;

            for (auto *d : ast->daten())
            {
                assert(d->art() == Ast_Knoten::AST_HEX);
                auto *daten = d->als<Ast_Hex *>();

                if (ast->z_daten() == 1)
                {
                    _laufwerk->schreiben_1byte(adresse, daten->wert());
                    adresse += 1;
                }
                else
                {
                    _laufwerk->schreiben_2byte(adresse, daten->wert());
                    adresse += 2;
                }
            }
        } break;

        default:
        {
            int x = 0;
        } break;
    }
}

}
