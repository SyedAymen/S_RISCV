#include "common_includes.h"
#include "memory.h"
#include "cpu.h"
#include "gui.h"

int main(int argc, char **argv)
{
  /* get binary data from first cmd-arg */
  FILE *f = fopen(argv[1], "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  uint8_t *program = (uint8_t *)malloc(fsize + 1);
  fread((void*)program, fsize, 1, f);
  fclose(f);

  /* load the program in memory */ 
  Memory* mem = MemoryGetFromProg(program);

  /* init the cpu after getting the program loaded */
  Cpu* cpu = CpuNew(mem);
  GuiWindowStart(cpu);
  /* free all used mallocs */
  free(cpu);
  free(mem);
  free(program);
  return 0;
}