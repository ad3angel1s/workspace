#include <stdio.h>
#include <stdlib.h>

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) do { fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
                                       __FILE__, __LINE__, __func__, ##args);} while(0)
#else
#define DEBUG_PRINT(fmt, args...) do { } while(0)
#endif

typedef struct _node {
  int val;
  struct _node *next;
} list_t;

void print_list(list_t *ptr)
{
  while (ptr) {
    printf("%d ", ptr->val);
    ptr = ptr->next;
  }
  printf("\n");
}

void add_node(list_t **ptr, int val)
{
  DEBUG_PRINT("Add node with value: %d, ptr: 0x%x, *ptr: 0x%x\n", val, ptr, *ptr);
  if (!*ptr) {
    *ptr = (list_t *) malloc(sizeof(list_t));
    (*ptr)->val = val;
    (*ptr)->next = NULL;
    DEBUG_PRINT("Added: ptr: 0x%x, *ptr: 0x%x, (*ptr)->val: %d, (*ptr)->next: 0x%x\n", ptr, *ptr, (*ptr)->val, (*ptr)->next);
    return;
  }

  list_t *tmp = *ptr;
  while (tmp->next) {
    tmp = tmp->next;
  }

  tmp->next = (list_t *) malloc(sizeof(list_t));
  tmp->next->val = val;
  tmp->next->next = NULL;
  DEBUG_PRINT("Added: ptr: 0x%x, tmp: 0x%x, tmp->next->val: %d, tmp>next->next: 0x%x\n", ptr, tmp, tmp->next->val, tmp->next->next);
}

void add_node_ordered(list_t **ptr, int val)
{
  if (!*ptr) {
    add_node(ptr, val);
  }
}

int main(void)
{
  int values[] = {1,2,3,4,5,6};
  size_t len = sizeof(values)/sizeof(int);
  int i;
  list_t *ptr = NULL;
  
  print_list(ptr);
  for (i=0; i<len; i++) {
    add_node(&ptr, values[i]);
  }
  print_list(ptr);

  return 0;
}
