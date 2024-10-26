#include "allgemein/zeichen.cpp"
#include "allgemein/position.cpp"
#include "allgemein/ergebnis.cpp"
#include "allgemein/fehler.cpp"
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
#include "asm/semantik.cpp"
#include "asm/emitter.cpp"

#include <fstream>
#include <sstream>
#include <iostream>

int main(int argc, char **argv)
{
    using namespace Asm;

    if (argc < 2)
    {
        std::cerr << "dateiname muss übergeben werden" << std::endl;
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
    for (auto knoten : ast)
    {
        knoten->ausgeben();
        printf("\n");
    }
#endif

    auto semantik = Semantik(ast);
    semantik.starten();

    auto speicher = new Vm::Arbeitsspeicher(256*256);
    auto emitter = Emitter(speicher, semantik.markierungen(), ast);
    emitter.starten();

    speicher->ausgeben(0, 32);

    auto cpu = Vm::Cpu(speicher);

    cpu.ausführen();
    cpu.ausgeben();

    return 0;
}

