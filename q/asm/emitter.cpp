#include "emitter.hpp"

#include "asm/ast.hpp"

#define ASM_STOPP()   __debugbreak()

namespace Asm {

Emitter::Emitter(Vm::Laufwerk *laufwerk, Ast ast)
    : _laufwerk(laufwerk)
    , _ast(ast)
{
}

void
Emitter::starten()
{
    for (auto *dekl : _ast.deklarationen)
    {
        deklaration_emittieren(dekl);
    }

    for (auto *anweisung : _ast.anweisungen)
    {
        anweisung_emittieren(anweisung);
    }
}

void
Emitter::deklaration_emittieren(Asm::Deklaration *dekl)
{
    switch (dekl->art())
    {
        case Deklaration::DATEN:
        {
            auto *ast = dekl->als<Daten *>();
            uint16_t adresse = ast->adresse;

            for (auto *daten : ast->daten())
            {
                if (ast->größe() == 1)
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
    }
}

void
Emitter::anweisung_emittieren(Asm::Anweisung *anweisung)
{
    auto *vm_anweisung = anweisung->anweisung();
    assert(vm_anweisung != nullptr);
    vm_anweisung->kodieren(_laufwerk, anweisung->adresse);
}

}
