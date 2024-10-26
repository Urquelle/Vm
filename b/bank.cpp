#include "allgemein/fehler.cpp"
#include "allgemein/ergebnis.cpp"
#include "vm/laufwerk.cpp"
#include "vm/arbeitsspeicher.cpp"
#include "vm/speicherbank.cpp"
#include "vm/vermittler.cpp"
#include "vm/operand.cpp"
#include "vm/anweisung.cpp"
#include "vm/cpu.cpp"

int main (int argc, char **argv)
{
    using namespace Vm;

    auto vermittler = new Vermittler();
    auto cpu = new Cpu(vermittler);

    size_t z_laufwerke = 8;
    size_t gr_laufwerk = 0xff;

    auto speicherbank = new Speicherbank(cpu, z_laufwerke, gr_laufwerk);
    auto speicher = new Arbeitsspeicher(0xff00);

    vermittler->zuordnen({
        .laufwerk = speicherbank,
        .anfang = 0x0,
        .ende = (uint16_t)gr_laufwerk
    });

    vermittler->zuordnen({
        .laufwerk = speicher,
        .anfang = (uint16_t)gr_laufwerk,
        .ende = 0xffff,
        .umschreiben = true
    });

    vermittler->schreiben_2byte(0, 1);
    printf("adr 0x0: %d\n", vermittler->lesen_2byte(0).wert());

    cpu->regs[REG_MB] = 1;
    printf("adr 0x0: %d\n", vermittler->lesen_2byte(0).wert());

    vermittler->schreiben_2byte(0, 42);
    cpu->regs[REG_MB] = 2;
    printf("adr 0x0: %d\n", vermittler->lesen_2byte(0).wert());

    cpu->regs[REG_MB] = 1;
    printf("adr 0x0: %d\n", vermittler->lesen_2byte(0).wert());

    cpu->regs[REG_MB] = 0;
    printf("adr 0x0: %d\n", vermittler->lesen_2byte(0).wert());

    return 0;
}

