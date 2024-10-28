#pragma once

#include "vm/anweisung.hpp"

namespace Asm {

class Emitter
{
public:
    Emitter(Vm::Laufwerk *laufwerk, Ast ast);

    void starten();
    void deklaration_emittieren(Asm::Deklaration *dekl);
    void anweisung_emittieren(Asm::Anweisung *anweisung);

private:
    Vm::Laufwerk *_laufwerk;
    Ast _ast;
};

}
