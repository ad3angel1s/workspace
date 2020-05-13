#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char lut[] = {'a','b','c','d','e','f','g','h','i','l','m','n','o','p','q','r'};

#define PASSWD_LEN (32)
#define NUM_CONSTRAINTS (2)
#define SIZE_CONSTRAINTS (3)
char must_have_chars[2][3] = {{'x','y','z'},{'A','B','C'}};

int genPasswd(char *passwd, int size);

int prng()
{
   return rand();
}

int main(void)
{
   srand(time(NULL));
   char generatedPassword[PASSWD_LEN+1] = {'\0'};

   (void) genPasswd(generatedPassword, PASSWD_LEN);
   printf("Generated random password is %s\n", generatedPassword);
   return 0;
}

int genPasswd(char *passwd, int size)
{
   int i,j,found,num_constraint, all_done = 1;

   int lut_size = sizeof(lut)/sizeof(char);

   int num_constraints_used[NUM_CONSTRAINTS] = {0};
   for (i=0; i<size; i++) {
      do {
          num_constraint = prng() % NUM_CONSTRAINTS;
          for (j=0; j<NUM_CONSTRAINTS; j++) {
             if (num_constraints_used[j] == 0) {
                all_done = 0;
             }
          } 
      } while (all_done == 0 && num_constraints_used[num_constraint] == 1);
      all_done = 1;
      num_constraints_used[num_constraint] = 1;

      passwd[i] = must_have_chars[num_constraint][prng() % SIZE_CONSTRAINTS];
   }

   return 0;
}
