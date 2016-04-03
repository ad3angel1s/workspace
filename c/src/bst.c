#include <stdio.h>
#include <stdlib.h>

typedef struct __node
{
  int val;
  struct __node *left;
  struct __node *right;
} Node;

void insert_node(Node **head, int val)
{
    if (*head == NULL)
    {
      Node *node = (Node *) malloc( sizeof(Node) );
	  
	  *head = node;
	  
	  node->val = val;
	  node->left = NULL;
	  node->right = NULL;
	  
	  printf("Inserting val = %d at addr 0x%llx\n", val, (unsigned long long int) node);
    }
    else
    {
      if ( (*head)->val > val )
	  {
	    insert_node( &(*head)->left, val );
	  }
	  else
	  {
            insert_node( &(*head)->right, val);
	  }
    }
    return;
}

void print_bst( Node **head )
{
    if (*head)
    {
      if ((*head)->right && (*head)->left)
	  {
        printf("Node val = %d, right = 0x%llx (%d), left = 0x%llx (%d)\n", (*head)->val,
	       (unsigned long long int) (*head)->right, (*head)->right->val,
	       (unsigned long long int) (*head)->left, (*head)->left->val);
	  }
	  else if ((*head)->right)
	  {
        printf("Node val = %d, right = 0x%llx (%d))\n", (*head)->val,
	       (unsigned long long int) (*head)->right, (*head)->right->val);
	  }
	  else if ((*head)->left)
	  {
		printf("Node val = %d, left = 0x%llx (%d))\n", (*head)->val,
		       (unsigned long long int) (*head)->left, (*head)->left->val);
	  }
	  else
	  {
		printf("Node val = %d... Leaf\n", (*head)->val);
	  }

      print_bst( &(*head)->right );
	  print_bst( &(*head)->left );
    }
    return;
}

int main()
{
  int values[9] = {8, 10, 3, 6, 1, 14, 13, 4, 7};
 
  Node *bst = NULL;

  printf("\n");
  for (int i=0; i<9; i++)
    {
      insert_node( &bst, values[i] );
    }  

  printf("\n");
  print_bst( &bst );

  printf("\n");
  return 0;
}
