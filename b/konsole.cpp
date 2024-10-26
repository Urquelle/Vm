#include "allgemein/fehler.cpp"
#include "allgemein/ergebnis.cpp"
#include "vm/laufwerk.cpp"
#include "vm/anzeige.cpp"
#include "vm/arbeitsspeicher.cpp"
#include "vm/vermittler.cpp"
#include "vm/operand.cpp"
#include "vm/anweisung.cpp"
#include "vm/cpu.cpp"

int main (int argc, char **argv)
{
    using namespace Vm;

    auto vermittler = new Vermittler();

    auto speicher = new Arbeitsspeicher(256*256);
    auto anzeige  = new Anzeige();

    vermittler->zuordnen({
        .laufwerk = speicher,
        .anfang = 0x0,
        .ende = 0xffff
    });

    vermittler->zuordnen({
        .laufwerk = anzeige,
        .anfang = 0x3000,
        .ende = 0x30ff,
        .umschreiben = true
    });

    auto cpu = new Cpu(vermittler);
    uint16_t adr = 0;

    #define VMOP(Opr) adr; adr += (Opr).wert()

    std::string text = "Hallo, Welt!";

    VMOP(Anweisung::Kodieren(speicher, OP_MOV_ADR_LIT, {Operand::Adr(0x3000), Operand::Lit(0xff << 8)}, adr));

    for (uint16_t i = 0; i <= 0xff; ++i)
    {
        uint8_t anweisung = (i % 2 == 0)
            ? 0x01
            : 0x02;

        VMOP(Anweisung::Kodieren(speicher, OP_MOV_ADR_LIT, {Operand::Adr(0x3000 + i), Operand::Lit(anweisung << 8 | '*')}, adr));
    }

    cpu->ausführen();

    return 0;
}

