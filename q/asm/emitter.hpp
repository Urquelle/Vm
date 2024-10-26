#pragma once

#include "vm/anweisung.hpp"

namespace Asm {

class Emitter
{
public:
    Emitter(Vm::Laufwerk *laufwerk, std::map<std::string, uint16_t> markierungen, std::vector<Ast_Knoten *> ast);

    void starten();
    void knoten_emittieren(Ast_Knoten *knoten);
    void mov_emittieren(Vm::Anweisung_Mov *anweisung);

private:
    Vm::Laufwerk *_laufwerk;
    std::map<std::string, uint16_t> _markierungen;
    std::vector<Ast_Knoten *> _ast;
};

}
