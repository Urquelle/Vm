#include "vm/anweisung.hpp"
#include "vm/cpu.hpp"

#include <cassert>

namespace Vm {

#define IF_SCHREIBE(A) if (!(A)) return false

uint16_t vorzeichen_erweitern(uint16_t x, uint8_t bit_count);
void     flags_setzen(Cpu *cpu, uint16_t r);

template<typename T>
T Anweisung::als()
{
    return static_cast<T> (this);
}

// erzeuger {{{

Anweisung *
Anweisung::Hlt()
{
    auto erg = new Anweisung_Hlt();

    return erg;
}

Anweisung *
Anweisung::Nop()
{
    auto erg = new Anweisung_Nop();

    return erg;
}

Anweisung *
Anweisung::Push(Operand *wert)
{
    auto erg = new Anweisung_Push(wert);

    return erg;
}

Anweisung *
Anweisung::Pop(Operand *reg)
{
    auto erg = new Anweisung_Pop(reg);

    return erg;
}

Anweisung *
Anweisung::Add(Operand *quelle1, Operand *quelle2)
{
    auto erg = new Anweisung_Add(quelle1, quelle2);

    return erg;
}

Anweisung *
Anweisung::Sub(Operand *quelle1, Operand *quelle2)
{
    auto erg = new Anweisung_Sub(quelle1, quelle2);

    return erg;
}

Anweisung *
Anweisung::Mul(Operand *quelle1, Operand *quelle2)
{
    auto erg = new Anweisung_Mul(quelle1, quelle2);

    return erg;
}

Anweisung *
Anweisung::Inc(Operand *op)
{
    auto erg = new Anweisung_Inc(op);

    return erg;
}

Anweisung *
Anweisung::Dec(Operand *op)
{
    auto erg = new Anweisung_Dec(op);

    return erg;
}

Anweisung *
Anweisung::Lsf(Operand *op1, Operand *op2)
{
    auto erg = new Anweisung_Lsf(op1, op2);

    return erg;
}

Anweisung *
Anweisung::Rsf(Operand *op1, Operand *op2)
{
    auto erg = new Anweisung_Rsf(op1, op2);

    return erg;
}

Anweisung *
Anweisung::And(Operand *op1, Operand *op2)
{
    auto erg = new Anweisung_And(op1, op2);

    return erg;
}

Anweisung *
Anweisung::Or(Operand *op1, Operand *op2)
{
    auto erg = new Anweisung_Or(op1, op2);

    return erg;
}

Anweisung *
Anweisung::Xor(Operand *op1, Operand *op2)
{
    auto erg = new Anweisung_Xor(op1, op2);

    return erg;
}

Anweisung *
Anweisung::Not(Operand *op)
{
    auto erg = new Anweisung_Not(op);

    return erg;
}

Anweisung *
Anweisung::Mov(Operand *ziel, Operand *quelle)
{
    auto erg = new Anweisung_Mov(ziel, quelle);

    return erg;
}

Anweisung *
Anweisung::Mov(Operand *ziel, Operand *basis, Operand *versatz)
{
    auto erg = new Anweisung_Mov(ziel, basis, versatz);

    return erg;
}

Anweisung *
Anweisung::Cal(Operand *op)
{
    auto erg = new Anweisung_Cal(op);

    return erg;
}

Anweisung *
Anweisung::Ret()
{
    auto erg = new Anweisung_Ret();

    return erg;
}

Anweisung *
Anweisung::Int(Operand *operand)
{
    auto erg = new Anweisung_Int(operand);

    return erg;
}

Anweisung *
Anweisung::RInt()
{
    auto erg = new Anweisung_RInt();

    return erg;
}

Anweisung *
Anweisung::Ldi(Operand *ziel, Operand *versatz)
{
    auto erg = new Anweisung_Ldi(ziel, versatz);

    return erg;
}

Anweisung *
Anweisung::Lea(Operand *ziel, Operand *versatz)
{
    auto erg = new Anweisung_Lea(ziel, versatz);

    return erg;
}

Anweisung *
Anweisung::Jne(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jne(lit, adresse);

    return erg;
}

Anweisung *
Anweisung::Jeq(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jeq(lit, adresse);

    return erg;
}

Anweisung *
Anweisung::Jlt(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jlt(lit, adresse);

    return erg;
}

Anweisung *
Anweisung::Jgt(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jgt(lit, adresse);

    return erg;
}

Anweisung *
Anweisung::Jle(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jle(lit, adresse);

    return erg;
}

Anweisung *
Anweisung::Jge(Operand *lit, Operand *adresse)
{
    auto erg = new Anweisung_Jge(lit, adresse);

    return erg;
}

// }}}
// dekodieren {{{

Anweisung *
Anweisung::Dekodieren(Cpu *cpu, uint8_t opcode)
{
    switch (opcode)
    {
        case OP_ADD_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Add(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_ADD_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Add(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_SUB_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Sub(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_SUB_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Sub(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_SUB_LIT_REG:
        {
            uint16_t lit = cpu->lesen2();
            uint8_t  reg = cpu->lesen();

            return Anweisung::Sub(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_MUL_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Mul(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_MUL_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Mul(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_INC_REG:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Inc(Operand::Reg(reg));
        } break;

        case OP_DEC_REG:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Dec(Operand::Reg(reg));
        } break;

        case OP_LSF_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Lsf(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_LSF_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Lsf(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_RSF_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Rsf(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_RSF_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Rsf(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_AND_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::And(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_AND_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::And(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_OR_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Or(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_OR_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Or(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_XOR_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Xor(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_XOR_REG_REG:
        {
            uint8_t reg1 = cpu->lesen();
            uint8_t reg2 = cpu->lesen();

            return Anweisung::Xor(Operand::Reg(reg1), Operand::Reg(reg2));
        } break;

        case OP_NOT:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Not(Operand::Reg(reg));
        } break;

        case OP_MOV_REG_LIT:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t lit = cpu->lesen2();

            return Anweisung::Mov(Operand::Reg(reg), Operand::Lit(lit));
        } break;

        case OP_MOV_REG_REG:
        {
            uint8_t zreg = cpu->lesen();
            uint8_t qreg = cpu->lesen();

            return Anweisung::Mov(Operand::Reg(zreg), Operand::Reg(qreg));
        } break;

        case OP_MOV_REG_ADR:
        {
            uint8_t  zreg = cpu->lesen();
            uint16_t qadr = cpu->lesen2();

            return Anweisung::Mov(Operand::Reg(zreg), Operand::Adr(qadr));
        } break;

        case OP_MOV_ADR_REG:
        {
            uint16_t zadr = cpu->lesen2();
            uint8_t  qreg = cpu->lesen();

            return Anweisung::Mov(Operand::Adr(zadr), Operand::Reg(qreg));
        } break;

        case OP_MOV_ADR_LIT:
        {
            uint16_t zadr = cpu->lesen2();
            uint16_t wert = cpu->lesen2();

            return Anweisung::Mov(Operand::Adr(zadr), Operand::Lit(wert));
        } break;

        case OP_MOV_REG_REG_ZGR:
        {
            uint8_t zreg = cpu->lesen();
            uint8_t qreg = cpu->lesen();

            return Anweisung::Mov(Operand::Reg(zreg), Operand::Zgr(Operand::Reg(qreg)));
        } break;

        case OP_MOV_REG_LIT_VER_REG:
        {
            uint8_t  zreg = cpu->lesen();
            uint16_t wert = cpu->lesen2();
            uint8_t  qreg = cpu->lesen();

            return Anweisung::Mov(Operand::Reg(zreg), Operand::Lit(wert), Operand::Reg(qreg));
        } break;

        case OP_PSH_LIT:
        {
            uint16_t wert = cpu->lesen2();

            return Anweisung::Push(Operand::Lit(wert));
        } break;

        case OP_PSH_REG:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Push(Operand::Reg(reg));
        } break;

        case OP_POP:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Pop(Operand::Reg(reg));
        } break;

        case OP_JNE_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jne(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_JNE_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jne(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JEQ_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jeq(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JEQ_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jeq(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_JLT_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jlt(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JLT_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jlt(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_JGT_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jgt(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JGT_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jgt(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_JLE_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jle(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JLE_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jle(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_JGE_REG:
        {
            uint8_t  reg = cpu->lesen();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jge(Operand::Reg(reg), Operand::Adr(adr));
        } break;

        case OP_JGE_LIT:
        {
            uint16_t lit = cpu->lesen2();
            uint16_t adr = cpu->lesen2();

            return Anweisung::Jge(Operand::Lit(lit), Operand::Adr(adr));
        } break;

        case OP_CAL_ADR:
        {
            uint16_t adr = cpu->lesen2();

            return Anweisung::Cal(Operand::Adr(adr));
        } break;

        case OP_CAL_REG:
        {
            uint8_t reg = cpu->lesen();

            return Anweisung::Cal(Operand::Reg(reg));
        } break;

        case OP_RET:
        {
            return Anweisung::Ret();
        } break;

        case OP_HLT:
        {
            return Anweisung::Hlt();
        } break;

        case OP_INT:
        {
            uint16_t wert = cpu->lesen2();

            return Anweisung::Int(Operand::Lit(wert));
        } break;

        case OP_RET_INT:
        {
            return Anweisung::RInt();
        } break;

        default:
        {
            assert(!"unbekannte anweisung");
        } break;
    }

    return Anweisung::Hlt();
}

// }}}
// konstruktoren {{{

Anweisung_Hlt::Anweisung_Hlt()
{
}

Anweisung_Nop::Anweisung_Nop()
{
}

Anweisung_Push::Anweisung_Push(Operand *op)
    : _op(op)
{
}

Anweisung_Pop::Anweisung_Pop(Operand *op)
    : _op(op)
{
}

Anweisung_Add::Anweisung_Add(Operand *quelle1, Operand *quelle2)
    : _quelle1(quelle1)
    , _quelle2(quelle2)
{
}

Anweisung_Sub::Anweisung_Sub(Operand *quelle1, Operand *quelle2)
    : _quelle1(quelle1)
    , _quelle2(quelle2)
{
}

Anweisung_Mul::Anweisung_Mul(Operand *quelle1, Operand *quelle2)
    : _quelle1(quelle1)
    , _quelle2(quelle2)
{
}

Anweisung_Lsf::Anweisung_Lsf(Operand *op1, Operand *op2)
    : _op1(op1)
    , _op2(op2)
{
}

Anweisung_Rsf::Anweisung_Rsf(Operand *op1, Operand *op2)
    : _op1(op1)
    , _op2(op2)
{
}

Anweisung_And::Anweisung_And(Operand *op1, Operand *op2)
    : _op1(op1)
    , _op2(op2)
{
}

Anweisung_Or::Anweisung_Or(Operand *op1, Operand *op2)
    : _op1(op1)
    , _op2(op2)
{
}

Anweisung_Xor::Anweisung_Xor(Operand *op1, Operand *op2)
    : _op1(op1)
    , _op2(op2)
{
}

Anweisung_Not::Anweisung_Not(Operand *op)
    : _op(op)
{
}

Anweisung_Mov::Anweisung_Mov(Operand *ziel, Operand *quelle)
    : _ziel(ziel)
    , _quelle(quelle)
    , _versatz(nullptr)
{
}

Anweisung_Mov::Anweisung_Mov(Operand *ziel, Operand *basis, Operand *versatz)
    : _ziel(ziel)
    , _basis(basis)
    , _versatz(versatz)
{
}

Anweisung_Cal::Anweisung_Cal(Operand *op)
    : _op(op)
{
}

Anweisung_Ret::Anweisung_Ret()
{
}

Anweisung_Inc::Anweisung_Inc(Operand *op)
    : _op(op)
{
}

Anweisung_Dec::Anweisung_Dec(Operand *op)
    : _op(op)
{
}

Anweisung_Int::Anweisung_Int(Operand *op)
    : _op(op)
{
}

Anweisung_RInt::Anweisung_RInt()
{
}

Anweisung_Ldi::Anweisung_Ldi(Operand *ziel, Operand *versatz)
    : _ziel(ziel)
    , _versatz(versatz)
{
}

Anweisung_Lea::Anweisung_Lea(Operand *ziel, Operand *versatz)
    : _ziel(ziel)
    , _versatz(versatz)
{
}

Anweisung_Jne::Anweisung_Jne(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

Anweisung_Jeq::Anweisung_Jeq(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

Anweisung_Jlt::Anweisung_Jlt(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

Anweisung_Jgt::Anweisung_Jgt(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

Anweisung_Jle::Anweisung_Jle(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

Anweisung_Jge::Anweisung_Jge(Operand *op, Operand *adresse)
    : _op(op)
    , _adr(adresse)
{
}

// }}}
// starten {{{

void
Anweisung_Add::starten(Cpu *cpu)
{
    if (_quelle1->art() == Operand::OPND_REG)
    {
        if (_quelle2->art() == Operand::OPND_REG)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Reg *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] + cpu->regs[q2];
        }
        else if (_quelle2->art() == Operand::OPND_LIT)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Lit *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] + q2;
        }
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Sub::starten(Cpu *cpu)
{
    if (_quelle1->art() == Operand::OPND_REG)
    {
        if (_quelle2->art() == Operand::OPND_REG)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Reg *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] - cpu->regs[q2];
        }
        else if (_quelle2->art() == Operand::OPND_LIT)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Lit *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] - q2;
        }
    }
    else if (_quelle1->art() == Operand::OPND_LIT)
    {
        if (_quelle2->art() == Operand::OPND_REG)
        {
            auto lit = _quelle1->als<Operand_Lit *>()->wert();
            auto reg = _quelle2->als<Operand_Reg *>()->wert();

            cpu->regs[REG_ACC] = lit - cpu->regs[reg];
        }
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Inc::starten(Cpu *cpu)
{
    auto reg = _op->als<Operand_Reg *>()->wert();
    auto wert = cpu->regs[reg];

    cpu->regs[reg] = wert + 1;

    flags_setzen(cpu, reg);
}

void
Anweisung_Dec::starten(Cpu *cpu)
{
    auto reg = _op->als<Operand_Reg *>()->wert();
    auto wert = cpu->regs[reg];

    cpu->regs[reg] = wert - 1;

    flags_setzen(cpu, reg);
}

void
Anweisung_Mul::starten(Cpu *cpu)
{
    if (_quelle1->art() == Operand::OPND_REG)
    {
        if (_quelle2->art() == Operand::OPND_REG)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Reg *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] * cpu->regs[q2];
        }
        else if (_quelle2->art() == Operand::OPND_LIT)
        {
            auto q1 = _quelle1->als<Operand_Reg *>()->wert();
            auto q2 = _quelle2->als<Operand_Lit *>()->wert();

            cpu->regs[REG_ACC] = cpu->regs[q1] * q2;
        }
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Lsf::starten(Cpu *cpu)
{
    auto reg = _op1->als<Operand_Reg *>()->wert();

    if (_op2->art() == Operand::OPND_LIT)
    {
        auto wert = _op2->als<Operand_Lit *>()->wert();

        cpu->regs[reg] = cpu->regs[reg] << wert;
    }
    else if (_op2->art() == Operand::OPND_REG)
    {
        auto reg2 = _op2->als<Operand_Lit *>()->wert();
        auto wert = cpu->regs[reg2];

        cpu->regs[reg] = cpu->regs[reg] << wert;
    }

    flags_setzen(cpu, reg);
}

void
Anweisung_Rsf::starten(Cpu *cpu)
{
    auto reg = _op1->als<Operand_Reg *>()->wert();

    if (_op2->art() == Operand::OPND_LIT)
    {
        auto wert = _op2->als<Operand_Lit *>()->wert();

        cpu->regs[reg] = cpu->regs[reg] >> wert;
    }
    else if (_op2->art() == Operand::OPND_REG)
    {
        auto reg2 = _op2->als<Operand_Lit *>()->wert();
        auto wert = cpu->regs[reg2];

        cpu->regs[reg] = cpu->regs[reg] >> wert;
    }

    flags_setzen(cpu, reg);
}

void
Anweisung_And::starten(Cpu *cpu)
{
    auto reg = _op1->als<Operand_Reg *>()->wert();

    if (_op2->art() == Operand::OPND_LIT)
    {
        auto wert = _op2->als<Operand_Lit *>()->wert();
        uint16_t erg = cpu->regs[reg] & wert;

        cpu->regs[REG_ACC] = erg;
    }
    else if (_op2->art() == Operand::OPND_REG)
    {
        auto reg2 = _op2->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg2];
        uint16_t erg = cpu->regs[reg] & wert;

        cpu->regs[REG_ACC] = erg;
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Or::starten(Cpu *cpu)
{
    auto reg = _op1->als<Operand_Reg *>()->wert();

    if (_op2->art() == Operand::OPND_LIT)
    {
        auto wert = _op2->als<Operand_Lit *>()->wert();
        uint16_t erg = cpu->regs[reg] | wert;

        cpu->regs[REG_ACC] = erg;
    }
    else if (_op2->art() == Operand::OPND_REG)
    {
        auto reg2 = _op2->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg2];
        uint16_t erg = cpu->regs[reg] | wert;

        cpu->regs[REG_ACC] = erg;
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Xor::starten(Cpu *cpu)
{
    auto reg = _op1->als<Operand_Reg *>()->wert();

    if (_op2->art() == Operand::OPND_LIT)
    {
        auto wert = _op2->als<Operand_Lit *>()->wert();
        uint16_t erg = cpu->regs[reg] ^ wert;

        cpu->regs[REG_ACC] = erg;
    }
    else if (_op2->art() == Operand::OPND_REG)
    {
        auto reg2 = _op2->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg2];
        uint16_t erg = cpu->regs[reg] ^ wert;

        cpu->regs[REG_ACC] = erg;
    }

    flags_setzen(cpu, REG_ACC);
}

void
Anweisung_Not::starten(Cpu *cpu)
{
    auto reg = _op->als<Operand_Reg *>()->wert();
    auto erg = ~(cpu->regs[reg]);

    cpu->regs[reg] = reg;

    flags_setzen(cpu, reg);
}

void
Anweisung_Push::starten(Cpu *cpu)
{
    switch (_op->art())
    {
        case Operand::OPND_REG:
        {
            auto reg = _op->als<Operand_Reg *>()->wert();
            auto wert = cpu->regs[reg];

            cpu->push(wert);
        } break;

        case Operand::OPND_LIT:
        {
            auto wert = _op->als<Operand_Lit *>()->wert();

            cpu->push(wert);
        } break;

        case  Operand::OPND_ADR:
        {
            auto wert = cpu->laufwerk->lesen_2byte(_op->als<Operand_Adr *>()->wert());

            if (wert.gut())
            {
                cpu->push(wert.wert());
            }
            else
            {
                assert(!"speicher konnte nicht gelesen werden");
            }
        } break;
    }
}

void
Anweisung_Pop::starten(Cpu *cpu)
{
    auto wert = cpu->pop();
    auto reg = _op->als<Operand_Reg *>()->wert();

    cpu->regs[reg] = wert;

    flags_setzen(cpu, reg);
}

void
Anweisung_Mov::starten(Cpu *cpu)
{
    if (_ziel->art() == Operand::OPND_REG)
    {
        auto zreg = _ziel->als<Operand_Reg *>()->wert();

        if (_quelle->art() == Operand::OPND_LIT)
        {
            // INFO: MOV_REG_LIT
            if (_versatz == nullptr)
            {
                auto wert = _quelle->als<Operand_Lit *>()->wert();

                cpu->regs[zreg] = wert;
            }

            // INFO: MOV_REG_LIT_VER_REG
            else
            {
                auto adr = _basis->als<Operand_Lit *>()->wert();
                auto versatz_reg = _versatz->als<Operand_Reg *>()->wert();
                auto versatz = cpu->regs[versatz_reg];
                auto wert = cpu->laufwerk->lesen_2byte(adr + versatz);

                if (wert.schlecht())
                {
                    assert(!"speicher lesefehler");
                }

                cpu->regs[zreg] = wert.wert();
            }
        }

        // INFO: MOV_REG_REG
        else if (_quelle->art() == Operand::OPND_REG)
        {
            auto qreg = _quelle->als<Operand_Reg *>()->wert();
            auto wert = cpu->regs[qreg];

            cpu->regs[zreg] = wert;
        }

        // INFO: MOV_REG_ADR
        else if (_quelle->art() == Operand::OPND_ADR)
        {
            auto adr = _quelle->als<Operand_Adr *>()->wert();
            auto wert = cpu->laufwerk->lesen_2byte(adr);

            if (wert.gut())
            {
                cpu->regs[zreg] = wert.wert();
            }
            else
            {
                assert(!"speicher konnte nicht gelesen werden");
            }
        }

        // INFO: MOV_REG_ZGR_REG
        else if (_quelle->art() == Operand::OPND_ZGR)
        {
            auto zgr = _quelle->als<Operand_Zgr *>()->wert();

            if (zgr->art() == Operand::OPND_REG)
            {
                auto adr = cpu->regs[zgr->als<Operand_Reg *>()->wert()];

                cpu->regs[zreg] = cpu->laufwerk->lesen_2byte(adr).wert();
            }
        }

        flags_setzen(cpu, _ziel->als<Operand_Reg *>()->wert());
    }
    else if (_ziel->art() == Operand::OPND_ADR)
    {
        // INFO: MOV_ADR_REG
        if (_quelle->art() == Operand::OPND_REG)
        {
            auto reg = _quelle->als<Operand_Reg *>()->wert();
            auto wert = cpu->regs[reg];
            auto adr = _ziel->als<Operand_Adr *>()->wert();

            cpu->laufwerk->schreiben_2byte(adr, wert);
        }

        // INFO: MOV_ADR_LIT
        else if (_quelle->art() == Operand::OPND_LIT)
        {
            auto wert = _quelle->als<Operand_Lit *>()->wert();
            auto adr = _ziel->als<Operand_Adr *>()->wert();

            cpu->laufwerk->schreiben_2byte(adr, wert);
        }
        else
        {
            assert(!"unbekannte anweisung");
        }
    }
}

void
Anweisung_Cal::starten(Cpu *cpu)
{
    uint16_t adr = 0;

    if (_op->art() == Operand::OPND_ADR)
    {
        adr = _op->als<Operand_Adr *>()->wert();
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();

        adr = cpu->regs[reg];
    }

    cpu->stand_speichern();

    cpu->regs[REG_IP] = adr;
}

void
Anweisung_Ret::starten(Cpu *cpu)
{
    cpu->stand_laden();
}

void
Anweisung_Int::starten(Cpu *cpu)
{
    auto wert = _op->als<Operand_Lit *>()->wert();
    auto interrupt_vektor_index = wert % 0xf;
    auto ist_maskiert = (bool) !((1 << interrupt_vektor_index) & cpu->regs[REG_IM]);

    if (ist_maskiert)
    {
        return;
    }

    auto interrupt_steuerung_adresse_zgr = cpu->interrupt_vektor_adresse + (interrupt_vektor_index * 2);
    auto interrupt_steuerung_adresse = cpu->laufwerk->lesen_2byte(interrupt_steuerung_adresse_zgr);

    if (!cpu->in_interrupt_steuerung)
    {
        // INFO: 0 ist die anzahl der argumente, die auf dem stack landen muss bevor stand_speichern
        cpu->push(0);
        cpu->stand_speichern();
    }

    if (interrupt_steuerung_adresse.schlecht())
    {
        assert(!"konnte wert nicht lesen");
    }

    cpu->in_interrupt_steuerung = true;
    cpu->regs[REG_IP] = interrupt_steuerung_adresse.wert();
}

void
Anweisung_RInt::starten(Cpu *cpu)
{
    cpu->in_interrupt_steuerung = false;
}

void
Anweisung_Ldi::starten(Cpu *cpu)
{
    uint16_t adresse = cpu->regs[REG_IP] + _versatz->als<Operand_Lit *>()->wert();

    auto wert = cpu->laufwerk->lesen_2byte(adresse);

    if (wert.gut())
    {
        assert(!"ups, beim lesen ist was schief gelaufen");
    }

    cpu->regs[_ziel->als<Operand_Reg *>()->wert()] = wert.wert();

    flags_setzen(cpu, _ziel->als<Operand_Reg *>()->wert());
}

void
Anweisung_Lea::starten(Cpu *cpu)
{
    cpu->regs[_ziel->als<Operand_Reg *>()->wert()] = cpu->regs[REG_IP] + _versatz->als<Operand_Lit *>()->wert();
}

void
Anweisung_Jne::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert != acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert != acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

void
Anweisung_Jeq::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert == acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert == acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

void
Anweisung_Jlt::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert < acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert < acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

void
Anweisung_Jgt::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert > acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert > acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

void
Anweisung_Jle::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert <= acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert <= acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

void
Anweisung_Jge::starten(Cpu *cpu)
{
    auto adr  = _adr->als<Operand_Adr *>()->wert();

    if (_op->art() == Operand::OPND_LIT)
    {
        auto wert = _op->als<Operand_Lit *>()->wert();
        auto acc  = cpu->regs[REG_ACC];

        if (wert >= acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
    else if (_op->art() == Operand::OPND_REG)
    {
        auto reg = _op->als<Operand_Reg *>()->wert();
        auto wert = cpu->regs[reg];
        auto acc = cpu->regs[REG_ACC];

        if (wert >= acc)
        {
            cpu->regs[REG_IP] = adr;
        }
    }
}

// }}}
// kodierung {{{

Ergebnis<uint8_t>
Anweisung::Kodieren(Laufwerk *speicher, uint16_t op, std::vector<Operand *> operanden, uint16_t adresse)
{
    switch (op)
    {
        case OP_ADD_REG_REG:  return Anweisung_Add ::Kodieren(speicher, operanden, adresse);
        case OP_ADD_REG_LIT:  return Anweisung_Add ::Kodieren(speicher, operanden, adresse);
        case OP_SUB_REG_REG:  return Anweisung_Sub ::Kodieren(speicher, operanden, adresse);
        case OP_SUB_REG_LIT:  return Anweisung_Sub ::Kodieren(speicher, operanden, adresse);
        case OP_SUB_LIT_REG:  return Anweisung_Sub ::Kodieren(speicher, operanden, adresse);
        case OP_MUL_REG_REG:  return Anweisung_Mul ::Kodieren(speicher, operanden, adresse);
        case OP_MUL_REG_LIT:  return Anweisung_Mul ::Kodieren(speicher, operanden, adresse);
        case OP_INC_REG    :  return Anweisung_Inc ::Kodieren(speicher, operanden, adresse);
        case OP_DEC_REG    :  return Anweisung_Dec ::Kodieren(speicher, operanden, adresse);
        case OP_LSF_REG_LIT:  return Anweisung_Lsf ::Kodieren(speicher, operanden, adresse);
        case OP_LSF_REG_REG:  return Anweisung_Lsf ::Kodieren(speicher, operanden, adresse);
        case OP_RSF_REG_LIT:  return Anweisung_Rsf ::Kodieren(speicher, operanden, adresse);
        case OP_RSF_REG_REG:  return Anweisung_Rsf ::Kodieren(speicher, operanden, adresse);
        case OP_AND_REG_LIT:  return Anweisung_And ::Kodieren(speicher, operanden, adresse);
        case OP_AND_REG_REG:  return Anweisung_And ::Kodieren(speicher, operanden, adresse);
        case OP_OR_REG_LIT :  return Anweisung_Or  ::Kodieren(speicher, operanden, adresse);
        case OP_OR_REG_REG :  return Anweisung_Or  ::Kodieren(speicher, operanden, adresse);
        case OP_XOR_REG_LIT:  return Anweisung_Xor ::Kodieren(speicher, operanden, adresse);
        case OP_XOR_REG_REG:  return Anweisung_Xor ::Kodieren(speicher, operanden, adresse);
        case OP_NOT        :  return Anweisung_Xor ::Kodieren(speicher, operanden, adresse);
        case OP_MOV_REG_LIT:  return Anweisung_Mov ::Kodieren(speicher, operanden, adresse);
        case OP_MOV_REG_REG:  return Anweisung_Mov ::Kodieren(speicher, operanden, adresse);
        case OP_MOV_REG_ADR:  return Anweisung_Mov ::Kodieren(speicher, operanden, adresse);
        case OP_MOV_ADR_REG:  return Anweisung_Mov ::Kodieren(speicher, operanden, adresse);
        case OP_MOV_ADR_LIT:  return Anweisung_Mov ::Kodieren(speicher, operanden, adresse);
        case OP_PSH_LIT    :  return Anweisung_Push::Kodieren(speicher, operanden, adresse);
        case OP_PSH_REG    :  return Anweisung_Push::Kodieren(speicher, operanden, adresse);
        case OP_POP        :  return Anweisung_Pop ::Kodieren(speicher, operanden, adresse);
        case OP_JNE_LIT    :  return Anweisung_Jne ::Kodieren(speicher, operanden, adresse);
        case OP_JNE_REG    :  return Anweisung_Jne ::Kodieren(speicher, operanden, adresse);
        case OP_JEQ_LIT    :  return Anweisung_Jeq ::Kodieren(speicher, operanden, adresse);
        case OP_JEQ_REG    :  return Anweisung_Jeq ::Kodieren(speicher, operanden, adresse);
        case OP_JLT_LIT    :  return Anweisung_Jlt ::Kodieren(speicher, operanden, adresse);
        case OP_JLT_REG    :  return Anweisung_Jlt ::Kodieren(speicher, operanden, adresse);
        case OP_JGT_LIT    :  return Anweisung_Jgt ::Kodieren(speicher, operanden, adresse);
        case OP_JGT_REG    :  return Anweisung_Jgt ::Kodieren(speicher, operanden, adresse);
        case OP_JLE_LIT    :  return Anweisung_Jle ::Kodieren(speicher, operanden, adresse);
        case OP_JLE_REG    :  return Anweisung_Jle ::Kodieren(speicher, operanden, adresse);
        case OP_JGE_LIT    :  return Anweisung_Jge ::Kodieren(speicher, operanden, adresse);
        case OP_JGE_REG    :  return Anweisung_Jge ::Kodieren(speicher, operanden, adresse);
        case OP_CAL_REG    :  return Anweisung_Cal ::Kodieren(speicher, operanden, adresse);
        case OP_CAL_ADR    :  return Anweisung_Cal ::Kodieren(speicher, operanden, adresse);
        case OP_RET        :  return Anweisung_Ret ::Kodieren(speicher, operanden, adresse);
        case OP_INT        :  return Anweisung_Int ::Kodieren(speicher, operanden, adresse);
        case OP_RET_INT    :  return Anweisung_RInt::Kodieren(speicher, operanden, adresse);

        default:              return Ergebnis<uint8_t>(Fehler());
    }
}

void
Anweisung_Hlt::kodieren(Laufwerk *laufwerk)
{
    //
}

void
Anweisung_Nop::kodieren(Laufwerk *laufwerk)
{
    //
}

void
Anweisung_Ldi::kodieren(Laufwerk *laufwerk)
{
    //
}

void
Anweisung_Lea::kodieren(Laufwerk *laufwerk)
{
    //
}

Ergebnis<uint8_t>
Anweisung_Add::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            auto reg1 = operanden[0]->als<Operand_Reg *>();
            auto reg2 = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_ADD_REG_REG));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg1->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, reg2->wert()));

            return 3;
        }
        else if (operanden[1]->art() == Operand::OPND_LIT)
        {
            auto reg = operanden[0]->als<Operand_Reg *>();
            auto lit = operanden[1]->als<Operand_Lit *>()->wert();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_ADD_REG_LIT));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg->wert()));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, lit));

            return 4;
        }
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Add::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Add::Kodieren(laufwerk, {_quelle1, _quelle2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Sub::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            auto reg1 = operanden[0]->als<Operand_Reg *>();
            auto reg2 = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_SUB_REG_REG));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg1->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, reg2->wert()));

            return 3;
        }
        else if (operanden[1]->art() == Operand::OPND_LIT)
        {
            auto reg = operanden[0]->als<Operand_Reg *>();
            auto lit = operanden[1]->als<Operand_Lit *>()->wert();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_SUB_REG_LIT));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg->wert()));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, lit));

            return 4;
        }
    }
    else if (operanden[0]->art() == Operand::OPND_LIT)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            auto lit = operanden[0]->als<Operand_Lit *>();
            auto reg = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_SUB_LIT_REG));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, reg->wert()));

            return 4;
        }
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Sub::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Sub::Kodieren(laufwerk, {_quelle1, _quelle2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Mul::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            auto reg1 = operanden[0]->als<Operand_Reg *>();
            auto reg2 = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MUL_REG_REG));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg1->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, reg2->wert()));

            return 3;
        }
        else if (operanden[1]->art() == Operand::OPND_LIT)
        {
            auto reg = operanden[0]->als<Operand_Reg *>();
            auto lit = operanden[1]->als<Operand_Lit *>()->wert();

            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MUL_REG_LIT));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg->wert()));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, lit));

            return 4;
        }
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Mul::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Mul::Kodieren(laufwerk, {_quelle1, _quelle2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Inc::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_INC_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));

        return 2;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Inc::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Inc::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Dec::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_DEC_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));

        return 2;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Dec::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Dec::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Lsf::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[1]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_LSF_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

        return 3;
    }
    else if (operanden[1]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_LSF_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Lsf::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Lsf::Kodieren(laufwerk, {_op1, _op2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Rsf::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[1]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_RSF_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

        return 3;
    }
    else if (operanden[1]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_RSF_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Rsf::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Rsf::Kodieren(laufwerk, {_op1, _op2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_And::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[1]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_AND_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

        return 3;
    }
    else if (operanden[1]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_AND_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_And::kodieren(Laufwerk *laufwerk)
{
    Anweisung_And::Kodieren(laufwerk, {_op1, _op2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Or::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[1]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_OR_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

        return 3;
    }
    else if (operanden[1]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_OR_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Or::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Or::Kodieren(laufwerk, {_op1, _op2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Xor::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[1]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_XOR_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

        return 3;
    }
    else if (operanden[1]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_XOR_REG_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Xor::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Xor::Kodieren(laufwerk, {_op1, _op2}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Not::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_NOT));
    IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Not::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Not::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Mov::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_REG_REG));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 2, operanden[1]->als<Operand_Reg *>()->wert()));

            return 4;
        }
        else if (operanden[1]->art() == Operand::OPND_LIT)
        {
            if (operanden.size() < 3)
            {
                IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_REG_LIT));
                IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
                IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));

                return 4;
            }
            else
            {
                IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_REG_LIT_VER_REG));
                IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
                IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Lit *>()->wert()));
                IF_SCHREIBE(speicher->schreiben_1byte(adresse + 4, operanden[2]->als<Operand_Reg *>()->wert()));
            }
        }
        else if (operanden[1]->art() == Operand::OPND_ADR)
        {
            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_REG_ADR));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, operanden[1]->als<Operand_Adr *>()->wert()));

            return 4;
        }
        else
        {
            return Fehler("unbekannte anweisung");
        }
    }
    else if (operanden[0]->art() == Operand::OPND_ADR)
    {
        if (operanden[1]->art() == Operand::OPND_REG)
        {
            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_ADR_REG));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, operanden[0]->als<Operand_Adr *>()->wert()));
            IF_SCHREIBE(speicher->schreiben_1byte(adresse + 3, operanden[1]->als<Operand_Reg *>()->wert()));

            return 4;
        }
        else if (operanden[1]->art() == Operand::OPND_LIT)
        {
            IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_MOV_ADR_LIT));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, operanden[0]->als<Operand_Adr *>()->wert()));
            IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, operanden[1]->als<Operand_Lit *>()->wert()));

            return 5;
        }
        else
        {
            return Fehler("unbekannte anweisung");
        }
    }
    else
    {
        return Fehler("unbekannte anweisung");
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Mov::kodieren(Laufwerk *laufwerk)
{
    if (_versatz == nullptr)
    {
        Anweisung_Mov::Kodieren(laufwerk, {_ziel, _quelle}, _adresse);
    }
    else
    {
        Anweisung_Mov::Kodieren(laufwerk, {_ziel, _quelle, _versatz}, _adresse);
    }
}

Ergebnis<uint8_t>
Anweisung_Push::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_PSH_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, operanden[0]->als<Operand_Lit *>()->wert()));

        return 3;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_PSH_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));

        return 2;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Push::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Push::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Pop::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_POP));
    IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, operanden[0]->als<Operand_Reg *>()->wert()));

    return 2;
}

void
Anweisung_Pop::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Pop::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jne::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JNE_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JNE_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jne::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jne::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jeq::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JEQ_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JEQ_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jeq::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jeq::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jlt::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JLT_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JLT_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jlt::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jlt::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jgt::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JGT_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JGT_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jgt::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jgt::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jle::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JLE_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JLE_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jle::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jle::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Jge::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 2)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_LIT)
    {
        auto lit = operanden[0]->als<Operand_Lit *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JGE_LIT));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, lit));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 3, adr));

        return 5;
    }
    else if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();
        auto adr = operanden[1]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_JGE_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 2, adr));

        return 4;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Jge::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Jge::Kodieren(laufwerk, {_op, _adr}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Cal::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    if (operanden[0]->art() == Operand::OPND_REG)
    {
        auto reg = operanden[0]->als<Operand_Reg *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_CAL_REG));
        IF_SCHREIBE(speicher->schreiben_1byte(adresse + 1, reg));

        return 2;
    }
    else if (operanden[0]->art() == Operand::OPND_ADR)
    {
        auto adr = operanden[0]->als<Operand_Adr *>()->wert();

        IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_CAL_ADR));
        IF_SCHREIBE(speicher->schreiben_2byte(adresse + 1, adr));

        return 3;
    }

    return Fehler("unbekannte anweisung");
}

void
Anweisung_Cal::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Cal::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Ret::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_RET));

    return 1;
}

void
Anweisung_Ret::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Ret::Kodieren(laufwerk, {}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_Int::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    if (operanden.size() < 1)
    {
        return Fehler("zu wenig argumente");
    }

    auto wert = operanden[0]->als<Operand_Lit *>()->wert();

    IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_INT));
    IF_SCHREIBE(speicher->schreiben_2byte(adresse, wert));

    return 3;
}

void
Anweisung_Int::kodieren(Laufwerk *laufwerk)
{
    Anweisung_Int::Kodieren(laufwerk, {_op}, _adresse);
}

Ergebnis<uint8_t>
Anweisung_RInt::Kodieren(Laufwerk *speicher, std::vector<Operand *> operanden, uint16_t adresse)
{
    IF_SCHREIBE(speicher->schreiben_1byte(adresse, OP_RET_INT));

    return 1;
}
void
Anweisung_RInt::kodieren(Laufwerk *laufwerk)
{
    Anweisung_RInt::Kodieren(laufwerk, {}, _adresse);
}

// }}}
// größe {{{
uint16_t
Anweisung_Mov::größe()
{
    uint16_t erg = 1 + _ziel->größe() + _quelle->größe();

    if (_versatz != nullptr)
    {
        erg += _versatz->größe();
    }

    return erg;
}
// }}}
// helfer {{{
uint16_t
vorzeichen_erweitern(uint16_t x, uint8_t z_bits)
{
    if ((x >> (z_bits - 1)) & 1)
    {
        x |= (0xFFFF << z_bits);
    }

    return x;
}

void
flags_setzen(Cpu *cpu, uint16_t reg)
{
    if (cpu->regs[reg] == 0)
    {
        cpu->regs[REG_FLAGS] = FL_ZRO;
    }

    // falls das linkeste bit eine 1 ist, dann ist
    // die zahl negativ
    else if (cpu->regs[reg] >> 15)
    {
        cpu->regs[REG_FLAGS] = FL_NEG;
    }
    else
    {
        cpu->regs[REG_FLAGS] = FL_POS;
    }
}

// }}}

}
