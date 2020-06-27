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
} node_t;

typedef struct _list {
  struct _node *head;
} list_t;

void print_list(list_t list)
{
  if (!list.head) {
    printf("Empty!\n");
    return;
  }

  node_t *ptr = list.head;
  while (ptr) {
    printf("%2d ", ptr->val);
    ptr = ptr->next;
  }
  printf("\n");
}

void add_node(list_t *list, int val)
{
  DEBUG_PRINT("Add node with value: %d\n", val);
  if (!list->head) {
    list->head = (node_t *) malloc(sizeof(node_t));
    list->head->val = val;
    list->head->next = NULL;
    DEBUG_PRINT("Added head: head: 0x%x, val: %d, next: 0x%x\n", list->head, list->head->val, list->head->next);
    return;
  }

  node_t *tmp = list->head;
  while (tmp->next) {
    tmp = tmp->next;
  }

  tmp->next = (node_t *) malloc(sizeof(node_t));
  tmp->next->val = val;
  tmp->next->next = NULL;
  DEBUG_PRINT("Added: node: 0x%x, val: %d, next: 0x%x\n", tmp->next, tmp->next->val, tmp->next->next);
}

void add_node_ordered(list_t *list, int val)
{
  if (!list->head) {
    return add_node(list, val);
  }

  node_t *tmp = list->head;
  while (tmp->next && tmp->next->val < val) {
    tmp = tmp->next;
  }

  /* Check if we are still on the head, if so, create a new head or create a new next */
  if (tmp == list->head) {
    if (tmp->val > val) {
      list->head = (node_t *) malloc(sizeof(node_t));
      list->head->val = val;
      list->head->next = tmp;
    } else {
      list->head->next = (node_t *) malloc(sizeof(node_t));
      list->head->next->val = val;
      list->head->next->next = NULL;
    }
    return;
  }

  node_t *next = tmp->next;
  /* We stopped because we didn't have any other value to check, so enqueue  */
  if (!tmp->next) {
    tmp->next = (node_t *) malloc(sizeof(node_t));
    tmp->next->val = val;
    tmp->next->next = NULL;
    return;
  }

  /* Otherwise insert before */
  tmp->next = (node_t *) malloc(sizeof(node_t));
  tmp->next->val = val;
  tmp->next->next = next;
}

void merge_node(list_t *list1, list_t *list2)
{
  node_t *tmp = list2->head;
  if (tmp) {
    list2->head = tmp->next;
  } else {
    list2->head = NULL;
  }
  /* at this point, the node that I want to insert is pointed by tmp and list2's head is popped */
  while (tmp) {
    DEBUG_PRINT("Inserting %d into the first list\n", tmp->val);
    /* iterate over the first list to find where to insert */
    node_t *curr = list1->head;
    node_t *prev = NULL;
    while (curr && curr->val < tmp->val) {
      prev = curr;
      curr = curr->next;
    }

    if (!prev) {
      /* curr->val must be greater (or equal) than tmp->val, so tmp becomes the new head */
      tmp->next = curr;
      list1->head = tmp;
    } else {
      /* tmp must be the new curr, i.e. prev->next */
      tmp->next = curr; /* prev->next */
      prev->next = tmp;
    }

    /* Move to the next node to insert in the second list */
    tmp = list2->head;
    if (tmp) {
      list2->head = tmp->next;
    } else {
      list2->head = NULL;
    }
  }
}

int main(void)
{
  int values[]  = {1,3,5,7,11,13};
  int values2[] = {0,2,4,6,8,10};
  size_t len = sizeof(values)/sizeof(int);
  size_t len2 = sizeof(values2)/sizeof(int);
  int i;
  list_t list1 = {.head = NULL};
  list_t list2 = {.head = NULL};

  printf("Original list: ");
  print_list(list1);
  for (i=0; i<len; i++) {
    add_node(&list1, values[i]);
  }
  printf("List after normal insertions: ");
  print_list(list1);
  add_node_ordered(&list1, 9);
  printf("List after ordered insertion: ");
  print_list(list1);

  for (i=0; i<len2; i++) {
    add_node_ordered(&list2, values2[i]);
  }
  printf("Another list after ordered insertions: ");
  print_list(list2);
  add_node_ordered(&list2, -2);
  printf("Another list with new head ordered: ");
  print_list(list2);

  merge_node(&list1, &list2);
  printf("New first list: ");
  print_list(list1);
  printf("New second list: ");
  print_list(list2);

  merge_node(&list2, &list1);
  printf("Swapped first list: ");
  print_list(list1);
  printf("Swapped second list: ");
  print_list(list2);

  return 0;
}
