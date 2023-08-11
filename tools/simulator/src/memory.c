#include "memory.h"
#include "stdbool.h"
#include <stdint.h>

uint8_t MemoryGetDataU8(Memory* mem, uint32_t addr) {
  /* if the addr in range of any memory segment then return the data there */
  for(int i = 0; i < mem->nSegs; i++) {
    if ((addr >= mem->listSegs[i]->addr)
          && (addr <= (mem->listSegs[i]->addr + mem->listSegs[i]->size))) {
      return mem->listSegs[i]->mem_arr[addr - mem->listSegs[i]->addr];
    }
  }
  printf("error: invalid memory read occurred at: %X", addr);
  exit(-1);
}

void MemorySetDataU8(Memory* mem, uint32_t addr, uint8_t value) {
  bool writeDone = false;
  for(int i = 0; i < mem->nSegs; i++) {
    if ((addr >= mem->listSegs[i]->addr)
          && (addr <= (mem->listSegs[i]->addr + mem->listSegs[i]->size))) {
      if (mem->listSegs[i]->flag == SEG_WRITE 
          || mem->listSegs[i]->flag == SEG_READ_AND_WRITE) {
        mem->listSegs[i]->mem_arr[addr - mem->listSegs[i]->addr] = value;
        writeDone = true;
      }
    }
  }
  if (!writeDone) {
    printf("error: invalid memory write occurred at: %X of: %X", addr, value);
    exit(0);
  }
}
uint16_t MemoryGetDataU16(Memory* mem, uint32_t addr) {
  return ((uint16_t)MemoryGetDataU8(mem, addr + 1) << 8) + (uint16_t)MemoryGetDataU8(mem, addr);
}

uint32_t MemoryGetDataU32(Memory* mem, uint32_t addr) {
  return ((uint32_t)MemoryGetDataU16(mem, addr + 2) << 16) + (uint32_t)MemoryGetDataU16(mem, addr);
}

void MemorySetDataU16(Memory * mem, uint32_t addr, uint16_t value) {
  MemorySetDataU8(mem, addr + 1, value >> 8);
  MemorySetDataU8(mem, addr, value);
}

void MemorySetDataU32(Memory * mem, uint32_t addr, uint32_t value) {
  MemorySetDataU16(mem, addr + 2, value >> 16);
  MemorySetDataU16(mem, addr, value);
}

uint8_t* MemoryGetPointerU8(Memory* mem, uint32_t addr) {
  for(int i = 0; i < mem->nSegs; i++) {
    if ((addr >= mem->listSegs[i]->addr)
          && (addr <= (mem->listSegs[i]->addr + mem->listSegs[i]->size))) {
      return &mem->listSegs[i]->mem_arr[addr - mem->listSegs[i]->addr];
    }
  }
  printf("error: invalid memory read occurred at: %X", addr);
  exit(-1);
}

Memory* MemoryGetFromProg(uint8_t *program) {
  /* extracting the ELF header */
  ElfHeader* hdr = (ElfHeader*)program; 
  
  /* init ELF program header list */
  ElfPhdr* phdrList[hdr->e_phnum];

  /* init Memory struct */
  Memory* mem = (Memory*)malloc(sizeof(Memory));

  /* finding the number of loadable segments */
  int t = 0;
  for (size_t i = 0; i < hdr->e_phnum; i++) {
    phdrList[i] = (ElfPhdr*)(program + hdr->e_phoff + (i * hdr->e_phentsize)); 
    if (phdrList[i]->p_type == 0x01 ) { /* Load type p_type == 1 */
      t++;
    }
  }
  if (t == 0) {
    printf("error: there is no loadable segment in program\n");
    return 0;
  }

  /* init the memory segments list that are loadable */
  mem->listSegs = (MemSegment**)malloc((t + 1) * sizeof(MemSegment*));
  mem->nSegs = 0;
  mem->enrtyPointAddr = hdr->e_entry;

  /* extracting the data to be loaded in memory */
  for (size_t i = 0; i < hdr->e_phnum; i++) {
    if (phdrList[i]->p_type == 0x01 ) { /* Load type p_type == 1 */
      mem->listSegs[mem->nSegs] = (MemSegment*)malloc(sizeof(MemSegment));
      mem->listSegs[mem->nSegs]->addr = phdrList[i]->p_vaddr;
      mem->listSegs[mem->nSegs]->size = phdrList[i]->p_memsz;
      mem->listSegs[mem->nSegs]->mem_arr = (program+phdrList[i]->p_offset);
      mem->listSegs[mem->nSegs]->flag = phdrList[i]->p_flags;
      mem->nSegs++;
      printf("addr: %x size %x", phdrList[i]->p_vaddr, phdrList[i]->p_memsz);
    }
  }
  return mem;
}