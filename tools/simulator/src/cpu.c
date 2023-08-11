#include "cpu.h"
#include "memory.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define RET_TRAP 40
#define MASK_OPCODE 0x0000007F
#define MASK_FUNCT3 0x00007000
#define MASK_FUNCT7 0xFE000000
#define MASK_RD 0x00000F80
#define MASK_RS1 0x000F8000
#define MASK_RS2 0x01F00000
#define DEBUG

#ifdef DEBUG
#define debug(x) printf((x))
#else
#define debug(x)
#endif

static bool trapOccured = false;

void undefinedInst(uint32_t inst) {
  printf("error: undefined instruction %X", inst);
  exit(-1);
}

void trapHandler(Cpu *cpu) {
  if (cpu->regs[17] == 1) {
    printf("%s", MemoryGetPointerU8(cpu->mem, cpu->regs[11]));
  }
}

void CsrWrite(Cpu *cpu, uint32_t val, uint32_t addr) {
  bool found = false;
  uint32_t *p = (uint32_t *)&cpu->csrRegs;
  for (int i = 0; i < NCSRS; i++) {
    if (csrRegsAddrList[i] == addr) {
      p += i;
      found = true;
      break;
    }
  }
  if (!found) {
    printf("csr not found %x\n", addr);
    exit(-1);
  }
  printf("got csr write addr %x, val %x\n", addr, val);
  *p = val;
}

uint32_t CsrRead(Cpu *cpu, uint32_t addr) {
  bool found = false;
  uint32_t *p = (uint32_t *)&cpu->csrRegs;
  for (int i = 0; i < NCSRS; i++) {
    if (csrRegsAddrList[i] == addr) {
      p += i;
      found = true;
      break;
    }
  }
  if (!found) {
    printf("csr not found %x\n", addr);
    exit(-1);
  }
  printf("got csr write addr %x, val %x\n", addr, *p);
  return *p;
}

uint32_t RD(uint32_t inst) { return (inst & MASK_RD) >> 7; }

uint32_t RS1(uint32_t inst) { return (inst & MASK_RS1) >> 15; }

uint32_t RS2(uint32_t inst) { return (inst & MASK_RS2) >> 20; }

uint32_t I_IMM(uint32_t inst) { return (int32_t)(inst & 0xfff00000) >> 20; }

uint32_t CSR(uint32_t inst) { return (uint32_t)(inst & 0xfff00000) >> 20; }

uint32_t S_IMM(uint32_t inst) {
  return ((int32_t)(inst & 0xfe000000) >> 20) | ((inst >> 7) & 0x1f);
}

uint32_t B_IMM(uint32_t inst) {
  return ((int32_t)(inst & 0x80000000) >> 19) | ((inst & 0x80) << 4) |
         ((inst >> 20) & 0x7e0) | ((inst >> 7) & 0x1e);
}

uint32_t U_IMM(uint32_t inst) { return (int32_t)(inst & 0xfffff000) >> 12; }

uint32_t J_IMM(uint32_t inst) {
  return (int32_t)((inst & 0x80000000) >> 11) | (inst & 0xff000) |
         ((inst >> 9) & 0x800) | ((inst >> 20) & 0x7fe);
}

uint32_t SHAMT(uint32_t inst) { return (uint32_t)(I_IMM(inst) & 0x1f); }

void addStack(Cpu *cpu) {
  MemSegment *stack = (MemSegment *)malloc(sizeof(MemSegment));
  stack->size = 1024;
  stack->flag = SEG_READ_AND_WRITE;
  stack->mem_arr = (uint8_t *)malloc(stack->size);
  stack->addr = 0xffffffff - stack->size;
  cpu->mem->listSegs[(cpu->mem->nSegs)++] = stack;
}

Cpu *CpuNew(Memory *mem) {
  Cpu *cpu = (Cpu *)calloc(1, sizeof(Cpu));
  cpu->mem = mem;
  cpu->pc = mem->enrtyPointAddr;
  addStack(cpu);
  return cpu;
}

int CpuStep(Cpu *cpu) {
  if (trapOccured) {
    trapHandler(cpu);
    trapOccured = false;
    return RET_TRAP;
  }

  cpu->inst = MemoryGetDataU32(cpu->mem, cpu->pc);

  bool foundInst = false;

  uint32_t opcode = MASK_OPCODE & cpu->inst;
  uint32_t funct3 = (MASK_FUNCT3 & cpu->inst) >> 12;
  uint32_t funct7 = (MASK_FUNCT7 & cpu->inst) >> 25;
  int i;
  for (i = 0; i < 40; i++) {
    if (i < 10) {
      if (instList[i].opcode == opcode && instList[i].funct3 == funct3 &&
          instList[i].funct7 == funct7) {
        instList[i].func(cpu);
        foundInst = true;
        break;
      }
    } else if (i == 36 || i == 35 || i == 33) {
      if (instList[i].opcode == opcode) {
        instList[i].func(cpu);
        foundInst = true;
        break;
      }
    } else {
      if (instList[i].opcode == opcode && instList[i].funct3 == funct3) {
        instList[i].func(cpu);
        foundInst = true;
        break;
      }
    }
  }

  if (cpu->inst == 0x00008067) {
    printf("reached the end of program\n");
    return -1;
  }

  if (!foundInst) {
    undefinedInst(cpu->inst);
    return -1;
  }
  cpu->pc += 4;
  cpu->regs[0] = 0;
  return i;
}

void ADD(Cpu *cpu) {
  debug("ADD\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] + cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void SUB(Cpu *cpu) {
  debug("SUB\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] - cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void XOR(Cpu *cpu) {
  debug("XOR\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] ^ cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void OR(Cpu *cpu) {
  debug("OR\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] | cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void AND(Cpu *cpu) {
  debug("AND\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] & cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void SLL(Cpu *cpu) {
  debug("SLL\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] << cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void SRL(Cpu *cpu) {
  debug("SRL\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = cpu->regs[rs1] >> cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void SRA(Cpu *cpu) {
  debug("SRA\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result = (int32_t)cpu->regs[rs1] >> cpu->regs[rs2];
  cpu->regs[rd] = result;
}

void SLT(Cpu *cpu) {
  debug("SLT\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result;
  if ((int32_t)cpu->regs[rs1] < (int32_t)cpu->regs[rs2]) {
    result = 1;
  } else {
    result = 0;
  }
  cpu->regs[rd] = result;
}

void SLTU(Cpu *cpu) {
  debug("SLTU\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t result;
  if (cpu->regs[rs1] < cpu->regs[rs2]) {
    result = 1;
  } else {
    result = 0;
  }
  cpu->regs[rd] = result;
}

void ADDI(Cpu *cpu) {
  debug("ADDI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] + (int32_t)imm;
  cpu->regs[rd] = result;
}

void XORI(Cpu *cpu) {
  debug("XORI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] ^ ((int32_t)imm & 0x0000fff);
  cpu->regs[rd] = result;
}

void ORI(Cpu *cpu) {
  debug("ORI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] | ((int32_t)imm & 0x0000fff);
  cpu->regs[rd] = result;
}

void ANDI(Cpu *cpu) {
  debug("ANDI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] & ((int32_t)imm & 0x0000fff);
  cpu->regs[rd] = result;
}

void SLLI(Cpu *cpu) {
  debug("SLLI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] << (0x0000001f & imm);
  cpu->regs[rd] = result;
}

void SRLI(Cpu *cpu) {
  debug("SRLI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = cpu->regs[rs1] >> (0x0000001f & imm);
  cpu->regs[rd] = result;
}

void SRAI(Cpu *cpu) {
  debug("SRAI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result = (int32_t)cpu->regs[rs1] >> (0x0000001f & imm);
  cpu->regs[rd] = result;
}

void SLTI(Cpu *cpu) {
  debug("SLTI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result;
  if ((int32_t)cpu->regs[rs1] < (int32_t)imm) {
    result = 1;
  } else {
    result = 0;
  }
  cpu->regs[rd] = result;
}

void SLTIU(Cpu *cpu) {
  debug("SLTIU\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t result;
  if ((uint32_t)cpu->regs[rs1] < (uint32_t)imm) {
    result = 1;
  } else {
    result = 0;
  }
  cpu->regs[rd] = result;
}

void LB(Cpu *cpu) {
  debug("LB\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t result = (int32_t)(int8_t)MemoryGetDataU8(cpu->mem, addr);
  cpu->regs[rd] = result;
}

void LW(Cpu *cpu) {
  debug("LW\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t result = MemoryGetDataU32(cpu->mem, addr);
  cpu->regs[rd] = result;
}

void LH(Cpu *cpu) {
  debug("LH\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t result = (int32_t)(int8_t)MemoryGetDataU16(cpu->mem, addr);
  cpu->regs[rd] = result;
}

void LBU(Cpu *cpu) {
  debug("LBU\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t result = (uint32_t)MemoryGetDataU8(cpu->mem, addr);
  cpu->regs[rd] = result;
}

void LHU(Cpu *cpu) {
  debug("LHU\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t result = (uint32_t)MemoryGetDataU16(cpu->mem, addr);
  cpu->regs[rd] = result;
}

void SB(Cpu *cpu) {
  debug("SB\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = S_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t data = cpu->regs[rs2];
  MemorySetDataU8(cpu->mem, addr, (uint8_t)data);
}

void SH(Cpu *cpu) {
  debug("SH\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = S_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t data = cpu->regs[rs2];
  MemorySetDataU16(cpu->mem, addr, (uint16_t)data);
}

void SW(Cpu *cpu) {
  debug("SW\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = S_IMM(cpu->inst);
  uint32_t addr = cpu->regs[rs1] + (int32_t)imm;
  uint32_t data = cpu->regs[rs2];
  MemorySetDataU32(cpu->mem, addr, data);
}

void BEQ(Cpu *cpu) {
  debug("BEQ\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  if (cpu->regs[rs1] == cpu->regs[rs2]) {
    cpu->pc +=
        imm - 4; // -4 cause in Step func we do pc=pc+4, dont wanna skip a inst
  }
}

void BNE(Cpu *cpu) {
  debug("BNE\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  printf("rs1 %x rs2 %x,imm %x, addr %x, prev pc %x\n", rs1, rs2, imm, imm,
         cpu->pc);

  if (cpu->regs[rs1] != cpu->regs[rs2]) {
    cpu->pc += imm - 4;
  }
  printf("cur pc %x\n", cpu->pc);
  fflush(stdout);
}

void BLT(Cpu *cpu) {
  debug("BLT\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  if ((int32_t)cpu->regs[rs1] < (int32_t)cpu->regs[rs2]) {
    cpu->pc += imm - 4;
  }
}

void BGE(Cpu *cpu) {
  debug("BGE\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  if ((int32_t)cpu->regs[rs1] >= (int32_t)cpu->regs[rs2]) {
    cpu->pc += imm - 4;
  }
}

void BLTU(Cpu *cpu) {
  debug("BLTU\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  if (cpu->regs[rs1] < cpu->regs[rs2]) {
    cpu->pc += imm - 4;
  }
}

void BGEU(Cpu *cpu) {
  debug("BGEU\n");
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t rs2 = RS2(cpu->inst);
  uint32_t imm = B_IMM(cpu->inst);
  if (cpu->regs[rs1] >= cpu->regs[rs2]) {
    cpu->pc += imm - 4;
  }
}

void JAL(Cpu *cpu) {
  debug("JAL\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t imm = J_IMM(cpu->inst);
  cpu->regs[rd] = cpu->pc + 4;
  cpu->pc = (cpu->pc - 4) + (int32_t)imm;
}

void JALR(Cpu *cpu) {
  debug("JALR\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t rs1 = RS1(cpu->inst);
  uint32_t imm = I_IMM(cpu->inst);
  cpu->regs[rd] = cpu->pc + 4;
  cpu->pc = cpu->regs[rs1] + (int32_t)imm;
}

void LUI(Cpu *cpu) {
  debug("LUI\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t imm = U_IMM(cpu->inst);
  cpu->regs[rd] = imm << 12;
}

void AUIPC(Cpu *cpu) {
  debug("AUIPC\n");
  uint32_t rd = RD(cpu->inst);
  uint32_t imm = U_IMM(cpu->inst);
  cpu->regs[rd] = cpu->pc + (imm << 12);
}

void ECALL(Cpu *cpu) {
  debug("ECALL\n");
  trapOccured = true;
}

void EBREAK(Cpu *cpu) { debug("EBREAK\n"); }

void CSRRWI(Cpu *cpu) { debug("CSRRWI\n"); }

void CSRRW(Cpu *cpu) {
  debug("CSRRW\n");
  uint32_t dest = RD(cpu->inst);
  uint32_t src = RS1(cpu->inst);
  uint32_t csr = CSR(cpu->inst);
  uint32_t t = CsrRead(cpu, csr);
  cpu->regs[dest] = t;
  CsrWrite(cpu, cpu->regs[src], csr);
}

void CSRRC(Cpu *cpu) {
  debug("CSRRC\n");
  uint32_t dest = RD(cpu->inst);
  uint32_t src = RS1(cpu->inst);
  uint32_t csr = CSR(cpu->inst);
  uint32_t t = CsrRead(cpu, csr);
  cpu->regs[dest] = t;
  CsrWrite(cpu, ~cpu->regs[src] & t, csr);
}

void CSRRCI(Cpu *cpu) { debug("CSRRCI\n"); }

void CSRRS(Cpu *cpu) {
  debug("CSRRS\n");
  uint32_t dest = RD(cpu->inst);
  uint32_t src = RS1(cpu->inst);
  uint32_t csr = CSR(cpu->inst);
  uint32_t t = CsrRead(cpu, csr);
  cpu->regs[dest] = t;
  CsrWrite(cpu, cpu->regs[src] | t, csr);
}

void CSRRSI(Cpu *cpu) { debug("CSRRSI\n"); }
