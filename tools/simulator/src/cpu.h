#pragma once

#include "common_includes.h"
#include "memory.h"

#define NCSRS 20

typedef struct {
  uint32_t mvendorid;
  uint32_t marchid;
  uint32_t mimpid;
  uint32_t mhartid;
  uint32_t mconfigptr;
  uint32_t mstatus;
  uint32_t misa;
  uint32_t medeleg;
  uint32_t mideleg;
  uint32_t mie;
  uint32_t mtvec;
  uint32_t mcounteren;
  uint32_t mstatush;
  uint32_t mscratch;
  uint32_t mepc;
  uint32_t mcause;
  uint32_t mtval;
  uint32_t mip;
  uint32_t mtinst;
  uint32_t mtval2;
} CsrRegs;

const static uint32_t csrRegsAddrList[] = {
    0xF11, 0xF12, 0xF13, 0xF14, 0xF15, 0x300, 0x301, 0x302, 0x303, 0x304,
    0x305, 0x306, 0x310, 0x340, 0x341, 0x342, 0x343, 0x344, 0x34A, 0x34B,
};

typedef struct {
  Memory *mem;
  uint32_t regs[32];
  uint32_t pc;
  CsrRegs csrRegs;
  // only used for simulator
  uint32_t inst;
} Cpu;

typedef struct {
  uint32_t opcode;
  uint32_t funct3;
  uint32_t funct7;
  void (*func)(Cpu *);
} Inst;

const static char InstName[][45] = {
    "ADD",   "SUB",   "XOR",    "OR",     "AND",   "SLL",   "SRL",    "SRA",
    "SLT",   "SLTU",  "ADDI",   "XORI",   "ORI",   "ANDI",  "SLLI",   "SRLI",
    "SRAI",  "SLTI",  "SLTIU",  "LB",     "LH",    "LW",    "LBU",    "LHU",
    "SB",    "SH",    "SW",     "BEQ",    "BNE",   "BLT",   "BGE",    "BLTU",
    "BGEU",  "JAL",   "JALR",   "LUI",    "AUIPC", "ECALL", "EBREAK", "CSRRW",
    "CSRRS", "CSRRC", "CSRRWI", "CSRRSI", "CSRRCI"};

static const char regNames[][32] = {
    "x0 (zero)", "x1 (ra)",  "x2 (sp)",    "x3 (gp)",   "x4 (tp)",  "x5 (t0)",
    "x6 (t1)",   "x7 (t2)",  "x8 (s0/fp)", "x9 (s1)",   "x10 (a0)", "x11 (a1)",
    "x12 (a2)",  "x13 (a3)", "x14 (a4)",   "x15 (a5)",  "x16 (a6)", "x17 (a7)",
    "x18 (s2)",  "x19 (s3)", "x20 (s4)",   "x21 (s5)",  "x22 (s6)", "x23 (s7)",
    "x24 (s8)",  "x25 (s9)", "x26 (s10)",  "x27 (s11)", "x28 (t3)", "x29 (t4)",
    "x30 (t5)",  "x31 (t6)",
};

static const char csrNames[][NCSRS] = {
    "mvendorid", "marchid",    "mimpid",   "mhartid",  "mconfigptr",
    "mstatus",   "misa",       "medeleg",  "mideleg",  "mie",
    "mtvec",     "mcounteren", "mstatush", "mscratch", "mepc",
    "mcause",    "mtval",      "mip",      "mtinst",   "mtval2",
};

void ADD(Cpu *cpu);
void SUB(Cpu *cpu);
void XOR(Cpu *cpu);
void OR(Cpu *cpu);
void AND(Cpu *cpu);
void SLL(Cpu *cpu);
void SRL(Cpu *cpu);
void SRA(Cpu *cpu);
void SLT(Cpu *cpu);
void SLTU(Cpu *cpu);
void ADDI(Cpu *cpu);
void XORI(Cpu *cpu);
void ORI(Cpu *cpu);
void ANDI(Cpu *cpu);
void SLLI(Cpu *cpu);
void SRLI(Cpu *cpu);
void SRAI(Cpu *cpu);
void SLTI(Cpu *cpu);
void SLTIU(Cpu *cpu);
void LB(Cpu *cpu);
void LW(Cpu *cpu);
void LH(Cpu *cpu);
void LBU(Cpu *cpu);
void LHU(Cpu *cpu);
void SB(Cpu *cpu);
void SH(Cpu *cpu);
void SW(Cpu *cpu);
void BEQ(Cpu *cpu);
void BNE(Cpu *cpu);
void BLT(Cpu *cpu);
void BGE(Cpu *cpu);
void BLTU(Cpu *cpu);
void BGEU(Cpu *cpu);
void JAL(Cpu *cpu);
void JALR(Cpu *cpu);
void LUI(Cpu *cpu);
void AUIPC(Cpu *cpu);
void ECALL(Cpu *cpu);
void EBREAK(Cpu *cpu);
void CSRRW(Cpu *cpu);
void CSRRS(Cpu *cpu);
void CSRRC(Cpu *cpu);
void CSRRWI(Cpu *cpu);
void CSRRSI(Cpu *cpu);
void CSRRCI(Cpu *cpu);

const static Inst instList[46] = {
    /* OPCODE  FUNCT3  FUCT7   FUNC */
    /*----------- R type --------- 0*/
    {0b0110011, 0x0, 0x00, ADD},
    {0b0110011, 0x0, 0x20, SUB},
    {0b0110011, 0x4, 0x00, XOR},
    {0b0110011, 0x6, 0x00, OR},
    {0b0110011, 0x7, 0x00, AND},
    {0b0110011, 0x1, 0x00, SLL},
    {0b0110011, 0x5, 0x00, SRL},
    {0b0110011, 0x5, 0x20, SRA},
    {0b0110011, 0x2, 0x00, SLT},
    {0b0110011, 0x3, 0x00, SLTU},
    /*--------- I1 type -------- 0 + 10 */
    {0b0010011, 0x0, 0x00, ADDI},
    {0b0010011, 0x4, 0x00, XORI},
    {0b0010011, 0x6, 0x00, ORI},
    {0b0010011, 0x7, 0x00, ANDI},
    {0b0010011, 0x1, 0x00, SLLI},
    {0b0010011, 0x5, 0x00, SRLI},
    {0b0010011, 0x5, 0x08, SRAI},
    {0b0010011, 0x2, 0x00, SLTI},
    {0b0010011, 0x3, 0x08, SLTIU},
    /*--------- I2 type -------- 10 + 9 */
    {0b0000011, 0x0, 0x00, LB},
    {0b0000011, 0x1, 0x00, LH},
    {0b0000011, 0x2, 0x00, LW},
    {0b0000011, 0x4, 0x00, LBU},
    {0b0000011, 0x5, 0x00, LHU},
    /*--------- S type -------- 19 + 5 */
    {0b0100011, 0x0, 0x00, SB},
    {0b0100011, 0x1, 0x00, SH},
    {0b0100011, 0x2, 0x00, SW},
    /*--------- B type -------- 24 + 3 */
    {0b1100011, 0x0, 0x00, BEQ},
    {0b1100011, 0x1, 0x00, BNE},
    {0b1100011, 0x4, 0x00, BLT},
    {0b1100011, 0x5, 0x00, BGE},
    {0b1100011, 0x6, 0x00, BLTU},
    {0b1100011, 0x7, 0x00, BGEU},
    /*--------- J type -------- 27 + 6 */
    {0b1101111, 0x0, 0x00, JAL},
    {0b1100111, 0x0, 0x00, JALR},
    /*--------- U type -------- 33 + 2 */
    {0b0110111, 0x0, 0x00, LUI},
    {0b0010111, 0x0, 0x00, AUIPC},
    /*---------- ECALL -------- 35 + 2 */
    {0b1110011, 0x0, 0x00, ECALL},
    {0b1110011, 0x0, 0x00, EBREAK},
    /*------------------------- 37 + 2 */
    {0b1110011, 0x1, 0x00, CSRRW},
    {0b1110011, 0x2, 0x00, CSRRS},
    {0b1110011, 0x3, 0x00, CSRRC},
    {0b1110011, 0x5, 0x00, CSRRWI},
    {0b1110011, 0x6, 0x00, CSRRSI},
    {0b1110011, 0x7, 0x00, CSRRCI},
    /*--------------------------39 + 6 */
};

Cpu *CpuNew(Memory *mem);
int CpuStep(Cpu *cpu);
