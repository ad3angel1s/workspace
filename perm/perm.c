#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE (10)

static unsigned perm_count = 0;

typedef struct {
  unsigned sz;
  unsigned val[ARRAY_SIZE];
} my_array_t;

void swap(my_array_t *v1, unsigned k1, my_array_t *v2, unsigned k2)
{
  unsigned tmp = v1->val[k1];
  v1->val[k1] = v2->val[k2];
  v2->val[k2] = tmp;
}

void print_array(my_array_t *v)
{
  int i = 0;
  printf("%u: ", perm_count++);
  for (i=0; i<v->sz; i++) {
    printf("%u ", v->val[i]);
  }
  printf("\n");
}

void perm(my_array_t *v, unsigned l)
{
  if (l == 1) {
    print_array(v);
  } else {
    perm(v, l-1);
    int i=0;
    for (i=0; i<l-1; i++) {
      if (!l%2) {
	swap(v, i, v, l-1);
      } else {
	swap(v, 0, v, l-1);
      }
      perm(v, l-1);
    }
  }
}

int main(int argc, char *argv[])
{
  my_array_t v = {.sz = ARRAY_SIZE, .val = {1,2,3,4,5,6,7,8,9,10}};
  unsigned user_size = ARRAY_SIZE;
  if (argc == 2) {
    user_size = atoi(argv[1]);
    if (user_size > ARRAY_SIZE) {
      printf("Input is too big, must be less or equal than 10\n");
      exit(1);
    }
  }
  v.sz = user_size;
  perm(&v, v.sz);
  return 0;
}
