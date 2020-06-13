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
  if (!ptr) {
    printf("Empty!\n");
    return;
  }

  while (ptr) {
    printf("%2d ", ptr->val);
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
    return add_node(ptr, val);
  }

  list_t *tmp = *ptr;
  while (tmp->next && tmp->next->val < val) {
    tmp = tmp->next;
  }

  list_t *next = tmp->next;
  if (!tmp->next) {
    tmp->next = (list_t *) malloc(sizeof(list_t));
    tmp->next->val = val;
    tmp->next->next = NULL;
    return;
  }

  if (tmp == *ptr) {
    list_t *previous_head = *ptr;
    *ptr = (list_t *) malloc(sizeof(list_t));
    (*ptr)->val = val;
    (*ptr)->next = previous_head;
    return;
  }

  tmp->next = (list_t *) malloc(sizeof(list_t));
  tmp->next->val = val;
  tmp->next->next = next;
}

void merge_node(list_t **ptr1, list_t **ptr2)
{
  list_t *tmp = *ptr2;
  while (*ptr2 = (*ptr2)->next) {
    /* iterate over the first list to find where to insert */
    list_t *curr = *ptr1;
    list_t *prev = NULL;
    while (curr && curr->val < tmp->val) {
      prev = curr;
      curr = curr->next;
    }

    list_t *next;
    if (!prev) {
      *ptr1 = tmp;
      (*ptr1)->next = curr;
    } else {
      next = prev->next;
      prev->next = tmp;
      prev->next->next = next;
    }

    /* Move to the next node to inser in the second list */
    tmp = *ptr2;
  }
}

int main(void)
{
  int values[]  = {1,3,5,7,11,13};
  int values2[] = {0,2,4,6,8,10};
  size_t len = sizeof(values)/sizeof(int);
  size_t len2 = sizeof(values2)/sizeof(int);
  int i;
  list_t *ptr = NULL;
  list_t *ptr2 = NULL;

  printf("Original list: ");
  print_list(ptr);
  for (i=0; i<len; i++) {
    add_node(&ptr, values[i]);
  }
  printf("List after normal insertions: ");
  print_list(ptr);
  add_node_ordered(&ptr, 9);
  printf("List after ordered insertion: ");
  print_list(ptr);

  for (i=0; i<len2; i++) {
    add_node_ordered(&ptr2, values2[i]);
  }
  printf("Another list after ordered insertions: ");
  print_list(ptr2);
  add_node_ordered(&ptr2, -2);
  printf("Another list with new head ordered: ");
  print_list(ptr2);

  merge_node(&ptr, &ptr2);
  printf("New first list: ");
  print_list(ptr);
  printf("New second list: ");
  print_list(ptr2);

  return 0;
}
