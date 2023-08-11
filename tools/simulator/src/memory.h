#pragma once
#include "common_includes.h"

typedef uint16_t HalfWord;
typedef uint32_t Word;
typedef uint32_t Address;
typedef uint32_t Offset;
#define EI_NIDENT 16

typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  HalfWord e_type;
  HalfWord e_machine;
  Word e_version;
  Address e_entry;
  Offset e_phoff;
  Offset e_shoff;
  Word e_flags;
  HalfWord e_ehsize;
  HalfWord e_phentsize;
  HalfWord e_phnum;
  HalfWord e_shentsize;
  HalfWord e_shnum;
  HalfWord e_shstrndx;
} ElfHeader;

typedef struct
{
  Word	p_type;
  Offset	p_offset;
  Address	p_vaddr;
  Address	p_paddr;
  Word	p_filesz;
  Word	p_memsz;
  Word	p_flags;
  Word	p_align;
} ElfPhdr;

typedef struct
{
  Word	sh_name;
  Word	sh_type;
  Word	sh_flags;
  Address	sh_addr;
  Offset	sh_offset;
  Word	sh_size;
  Word	sh_link;
  Word	sh_info;
  Word	sh_addralign;
  Word	sh_entsize;
} ElfShdr;

typedef enum {
  SEG_READ = 0x4,
  SEG_WRITE = 0x2,
  SEG_READ_AND_WRITE = 0x6,
  /* more options exist but for now we arent 
  concerned about them
   SEG_EXECUTE */
}SegFlags;

typedef struct {
  uint32_t addr;
  uint32_t size;
  SegFlags flag;
  uint8_t* mem_arr;
} MemSegment;

typedef struct {
  MemSegment **listSegs;
  int nSegs;
  uint32_t enrtyPointAddr;
} Memory;

Memory* MemoryGetFromProg(uint8_t *program);

uint8_t MemoryGetDataU8(Memory* mem, uint32_t addr);
uint16_t MemoryGetDataU16(Memory* mem, uint32_t addr);
uint32_t MemoryGetDataU32(Memory* mem, uint32_t addr);

uint8_t* MemoryGetPointerU8(Memory* mem, uint32_t addr);

void MemorySetDataU8(Memory* mem, uint32_t addr, uint8_t value);
void MemorySetDataU16(Memory* mem, uint32_t addr, uint16_t value);
void MemorySetDataU32(Memory* mem, uint32_t addr, uint32_t value);
