#include <setjmp.h>
#include <stdio.h>

int main() {
  jmp_buf env;
  volatile int a = 69;
  int b = 42;

  if(!setjmp(env)) { b=0; a = 0; longjmp(env,1); }
  else {printf("a is %d and b is %d\n", a, b);}
  return 0;
}
