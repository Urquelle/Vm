#pragma once

#include <cstdint>
#include <vector>

#include "operand.hpp"
#include "laufwerk.hpp"
#include "cpu.hpp"

namespace Vm {

#define Op_Liste \
    X(OP_HLT,                 0, "hlt") \
    /* mov */                           \
    X(OP_MOV_REG_LIT,         1, "mov") \
    X(OP_MOV_REG_REG,         2, "mov") \
    X(OP_MOV_REG_ADR,         3, "mov") \
    X(OP_MOV_ADR_REG,         4, "mov") \
    X(OP_MOV_ADR_LIT,         5, "mov") \
    X(OP_MOV_REG_REG_ZGR,     6, "mov") \
    X(OP_MOV_REG_LIT_VER_REG, 7, "mov") \
    /* arithmetik */                    \
    X(OP_ADD_REG_REG,         8, "add") \
    X(OP_ADD_REG_LIT,         9, "add") \
    X(OP_SUB_REG_REG,        10, "sub") \
    X(OP_SUB_REG_LIT,        11, "sub") \
    X(OP_SUB_LIT_REG,        12, "sub") \
    X(OP_INC_REG,            13, "inc") \
    X(OP_DEC_REG,            14, "dec") \
    X(OP_MUL_REG_REG,        15, "mul") \
    X(OP_MUL_REG_LIT,        16, "mul") \
    /* ... */                           \
    X(OP_PSH_LIT,            17, "psh") \
    X(OP_PSH_REG,            18, "psh") \
    X(OP_POP,                19, "pop") \
    /* verzweigung */                   \
    X(OP_JNE_LIT,            20, "jne") \
    X(OP_JNE_REG,            21, "jne") \
    X(OP_JEQ_REG,            22, "jeq") \
    X(OP_JEQ_LIT,            23, "jeq") \
    X(OP_JLT_REG,            24, "jlt") \
    X(OP_JLT_LIT,            25, "jlt") \
    X(OP_JGT_REG,            26, "jgt") \
    X(OP_JGT_LIT,            27, "jgt") \
    X(OP_JLE_REG,            28, "jle") \
    X(OP_JLE_LIT,            29, "jle") \
    X(OP_JGE_REG,            30, "jge") \
    X(OP_JGE_LIT,            31, "jge") \
    /* ... */                           \
    X(OP_CAL_ADR,            32, "cal") \
    X(OP_CAL_REG,            33, "cal") \
    X(OP_RET,                34, "ret") \
    /* shift */                         \
    X(OP_LSF_REG_LIT,        35, "lsf") \
    X(OP_LSF_REG_REG,        36, "lsf") \
    X(OP_RSF_REG_LIT,        37, "rsf") \
    X(OP_RSF_REG_REG,        38, "rsf") \
    /* logik */                         \
    X(OP_AND_REG_LIT,        39, "and") \
    X(OP_AND_REG_REG,        40, "and") \
    X(OP_OR_REG_LIT,         41, "or")  \
    X(OP_OR_REG_REG,         42, "or")  \
    X(OP_XOR_REG_LIT,        43, "xor") \
    X(OP_XOR_REG_REG,        44, "xor") \
    /* ... */                           \
    X(OP_NOT,                45, "not") \
    X(OP_INT,                46, "int") \
    X(OP_RTI,                47, "rti") \
    X(OP_BRK,                48, "brk")

enum Op : std::int8_t
{
    #define X(Name, Wert, ...) Name = Wert,
    Op_Liste
    #undef X

    Z_OPS,
};

class Anweisung
{
public:
    enum Art
    {
        HLT,
        NOP,
        MOV,
        ADD,
        SUB,
        INC,
        DEC,
        MUL,
        PSH,
        POP,
        JNE,
        JEQ,
        JLT,
        JGT,
        JLE,
        JGE,
        CAL,
        RET,
        LSF,
        RSF,
        AND,
        OR,
        XOR,
        NOT,
        INT,
        RTI,
        BRK,

        LDI,
        LEA
    };

    static Anweisung * Hlt();
    static Anweisung * Brk();
    static Anweisung * Nop();
    static Anweisung * Mov(Operand *ziel, Operand *quelle);
    static Anweisung * Mov(Operand *ziel, Operand *basis, Operand *versatz);
    static Anweisung * Push(Operand *op);
    static Anweisung * Pop(Operand *op);
    static Anweisung * Add(Operand *q1, Operand *q2);
    static Anweisung * Sub(Operand *q1, Operand *q2);
    static Anweisung * Mul(Operand *q1, Operand *q2);
    static Anweisung * Inc(Operand *op);
    static Anweisung * Dec(Operand *op);
    static Anweisung * Lsf(Operand *op1, Operand *op2);
    static Anweisung * Rsf(Operand *op1, Operand *op2);
    static Anweisung * And(Operand *op1, Operand *op2);
    static Anweisung * Or(Operand *op1, Operand *op2);
    static Anweisung * Xor(Operand *op1, Operand *op2);
    static Anweisung * Not(Operand *op);
    static Anweisung * Cal(Operand *op);
    static Anweisung * Ret();
    static Anweisung * Jne(Operand *op, Operand *ziel);
    static Anweisung * Jeq(Operand *op, Operand *ziel);
    static Anweisung * Jlt(Operand *op, Operand *ziel);
    static Anweisung * Jgt(Operand *op, Operand *ziel);
    static Anweisung * Jle(Operand *op, Operand *ziel);
    static Anweisung * Jge(Operand *op, Operand *ziel);
    static Anweisung * Int(Operand *id);
    static Anweisung * Rti();
    static Anweisung * Ldi(Operand *ziel, Operand *versatz);
    static Anweisung * Lea(Operand *ziel, Operand *versatz);

    static Anweisung * Dekodieren(Cpu *cpu, uint8_t opcode);
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, uint16_t op, std::vector<Operand *> operanden, uint16_t adresse);

    virtual void       starten(Cpu *cpu) = 0;
    virtual void       kodieren(Laufwerk *laufwerk, uint16_t adresse) = 0;
    virtual uint16_t   größe() = 0;

    Anweisung(Art art) : _art(art) {}
    Art art() { return _art; }

    template<typename T> T als();

private:
    Art _art;
};

class Anweisung_Hlt : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Hlt();

    void starten(Cpu *cpu) override { cpu->halt = true; }
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1; }
};

class Anweisung_Brk : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Brk();

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1; }
};

class Anweisung_Nop : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Nop();

    void starten(Cpu *cpu) override {}
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1; }
};

class Anweisung_Push : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Push(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Pop : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Pop(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Add : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Add(Operand *quelle1, Operand *quelle2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _quelle1->größe() + _quelle2->größe(); }

private:
    Operand *_quelle1;
    Operand *_quelle2;
};

class Anweisung_Sub : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Sub(Operand *quelle1, Operand *quelle2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _quelle1->größe() + _quelle2->größe(); }

private:
    Operand *_quelle1;
    Operand *_quelle2;
};

class Anweisung_Mul : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Mul(Operand *quelle1, Operand *quelle2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _quelle1->größe() + _quelle2->größe(); }

private:
    Operand *_quelle1;
    Operand *_quelle2;
};

class Anweisung_Lsf : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Lsf(Operand *op1, Operand *op2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op1->größe() + _op2->größe(); }

private:
    Operand *_op1;
    Operand *_op2;
};

class Anweisung_Rsf : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Rsf(Operand *op1, Operand *op2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op1->größe() + _op2->größe(); }

private:
    Operand *_op1;
    Operand *_op2;
};

class Anweisung_And : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_And(Operand *op1, Operand *op2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op1->größe() + _op2->größe(); }

private:
    Operand *_op1;
    Operand *_op2;
};

class Anweisung_Or : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Or(Operand *op1, Operand *op2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op1->größe() + _op2->größe(); }

private:
    Operand *_op1;
    Operand *_op2;
};

class Anweisung_Xor : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Xor(Operand *op1, Operand *op2);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op1->größe() + _op2->größe(); }

private:
    Operand *_op1;
    Operand *_op2;
};

class Anweisung_Not : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Not(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Cal : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Cal(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Ret : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Ret();

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1; }
};

class Anweisung_Inc : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Inc(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Dec : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Dec(Operand *op);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Mov : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Mov(Operand *ziel, Operand *quelle);
    Anweisung_Mov(Operand *ziel, Operand *basis, Operand *versatz);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override;

private:
    Operand *_ziel;
    union {
        Operand *_quelle;
        Operand *_basis;
    };
    Operand *_versatz;
};

class Anweisung_Int : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Int(Operand *operand);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe(); }

private:
    Operand *_op;
};

class Anweisung_Rti : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Rti();

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1; }
};

class Anweisung_Ldi : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Ldi(Operand *ziel, Operand *versatz);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _ziel->größe() + _versatz->größe(); }

private:
    Operand *_ziel;
    Operand *_versatz;
};

class Anweisung_Lea : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Lea(Operand *ziel, Operand *versatz);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _ziel->größe() + _versatz->größe(); }

private:
    Operand *_ziel;
    Operand *_versatz;
};

class Anweisung_Jne : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jne(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

class Anweisung_Jeq : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jeq(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

class Anweisung_Jlt : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jlt(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

class Anweisung_Jgt : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jgt(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

class Anweisung_Jle : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jle(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

class Anweisung_Jge : public Anweisung
{
public:
    static Ergebnis<uint8_t> Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse);

    Anweisung_Jge(Operand *op, Operand *adresse);

    void starten(Cpu *cpu) override;
    void kodieren(Laufwerk *laufwerk, uint16_t adresse) override;
    uint16_t größe() override { return 1 + _op->größe() + _adr->größe(); }

private:
    Operand *_op;
    Operand *_adr;
};

}
