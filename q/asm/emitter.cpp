#include "emitter.hpp"

#include "asm/ast.hpp"

#define ASM_STOPP()   __debugbreak()

namespace Asm {

Emitter::Emitter(Vm::Laufwerk *laufwerk, std::map<std::string, uint16_t> markierungen, std::vector<Ast_Knoten *> ast)
    : _laufwerk(laufwerk)
    , _markierungen(markierungen)
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

        default:
        {
            int x = 0;
        } break;
    }
}

}
