#include "allgemein/zeichen.cpp"
#include "allgemein/position.cpp"
#include "allgemein/ergebnis.cpp"
#include "allgemein/fehler.cpp"
#include "allgemein/diagnostik.cpp"
#include "vm/laufwerk.cpp"
#include "vm/arbeitsspeicher.cpp"
#include "vm/vermittler.cpp"
#include "vm/operand.cpp"
#include "vm/anweisung.cpp"
#include "vm/cpu.cpp"
#include "asm/token.cpp"
#include "asm/lexer.cpp"
#include "asm/ast.cpp"
#include "asm/syntax.cpp"
#include "asm/symbol.cpp"
#include "asm/zone.cpp"
#include "asm/semantik.cpp"
#include "asm/emitter.cpp"

#include <fstream>
#include <sstream>
#include <iostream>

int main(int argc, char **argv)
{
    using namespace Asm;

    if (argc < 3)
    {
        std::cerr << "dateiname und startadresse müssen übergeben werden" << std::endl;
        exit(1);
    }

    auto datei_name = argv[1];
    std::ifstream t(datei_name);
    std::stringstream text;
    text << t.rdbuf();

    auto lex = Lexer(datei_name, text.str());
    auto tokens = lex.starten();

#ifdef ASM_TOKEN_AUSGEBEN
    for (auto token : tokens)
    {
        token->ausgeben();
        printf("\n");
    }
#endif

    auto syntax = Syntax(tokens);
    auto ast = syntax.starten();

#ifdef ASM_AST_AUSGEBEN
    for (auto knoten : ast.deklarationen)
    {
        knoten->ausgeben(0, std::cout);
        printf("\n");
    }

    for (auto knoten : ast.anweisungen)
    {
        knoten->ausgeben(0, std::cout);
        printf("\n");
    }
#endif

    if (syntax.diagnostik().hat_meldungen())
    {
        for (auto meldung : syntax.diagnostik().meldungen())
        {
             std::cout << meldung << std::endl;
        }
    }

    auto semantik = Semantik(ast);
    semantik.starten();

    if (semantik.diagnostik().hat_meldungen())
    {
        for (auto meldung : semantik.diagnostik().meldungen())
        {
             std::cout << meldung << std::endl;
        }
    }

    auto speicher = new Vm::Arbeitsspeicher(256*256);
    auto emitter = Emitter(speicher, ast);
    emitter.starten();

    speicher->ausgeben(0, 32);

    auto cpu = Vm::Cpu(speicher);

    uint16_t adresse = atoi(argv[2]);
    cpu.ausführen(adresse);
    cpu.ausgeben();

    return 0;
}

