void _trapHandler();

int c = 0;

void _entry() {
  void *trapHadler = &_trapHandler;
  asm volatile("csrw mtvec, %0" : "=r"(trapHadler));
}

void _trapHandler() {
  unsigned int a1 = 0;

  // TODO: is this the right way to get register values???
  asm volatile("mv %0, a1" : "=r"(a1));

  // TODO: testing puropse only
  if (a1) {
    c = 0xff;
  } else {
    c = 0xaa;
  }
}

// void _Exit(int exit_code) __attribute__ ((noreturn,noinline));
// void _Exit(int exit_code) {
//     (void)exit_code;
//     // Halt
//     while (1) {
//         __asm__ ("nop");
//     }
// }

// int main() {
//     return 0;
// }

// extern void _start(void) __attribute__ ((noreturn));
// void _start(void) {
//     int rc = main();

//     _Exit(rc);
// }

/* void _start(void) { */
/*   char *buf = "i miss you raaaaaaaaaaaaaa\n"; */
/*   asm volatile("mv a1, %0" : "=r"(buf)); */
/*   asm volatile("addi a7, zero, 1"); */
/*   asm("ecall"); */
/* } */

/*
int k(int a, int b ){
    return a + b;
}

void _start(void) {
    int a = 5, b = 3;
    k(a, b);
}
*/
void _start(void) {
  int a = 0xff;
  asm volatile("mv t0, %0" : "=r"(a));
  asm volatile("csrw mtvec, t0");
}
