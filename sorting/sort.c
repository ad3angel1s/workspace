#include <stdio.h>
#include <string.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void print_vec(int *v, size_t sz)
{
   int i;
   for (i=0; i<sz; i++)
      printf("%d ", v[i]);
   printf("\n");
}

void bubble_sort_vec(int *v, size_t sz)
{
   int i,j;

   for (i=0; i<sz; i++) {
      for (j=i+1; j<sz; j++) {
         if (v[i] > v[j]) {
            int tmp = v[i];
            v[i] = v[j];
            v[j] = tmp;
         }
      }
   }
   return;
}

void merge_vec(int *, int, int, int, int);
void merge_sort_vec(int *v, size_t sz)
{
   int i,j;

   for (i=1; i<sz; i*=2) {
      for (j=0; j<sz-2*i; j+=2*i) {
         merge_vec(v, j, i, j+i, i);
      }
   }
}

void merge_vec(int *v, int start_v1, int len_v1, int start_v2, int len_v2)
{
    int i = 0, j = 0, k = 0;
    int tmp[len_v1+len_v2];

    while (i < len_v1 || j < len_v2) {
       if (i == len_v1) {
         tmp[k++] = v[start_v2 + j++];
       } else if (j == len_v2) {
         tmp[k++] = v[start_v1 + i++];
       } else {
          if (v[start_v1 + i] > v[start_v2 + j]) {
             tmp[k++] = v[start_v2 + j++];
          } else {
             tmp[k++] = v[start_v1 + i++];
          }
       }
    }

    for (i=0; i<len_v1+len_v2; i++) {
       v[start_v1 + i] = tmp[i];
    }

    return;
}

int main(void)
{
   int v1[] = {3,4,2,1,7,9,8,0,6,5};
   int v2[] = {3,4,2,1,7,9,8,0,0,0};
   printf("v1: ");
   print_vec(v1,10);
   printf("v2: ");
   print_vec(v2,10);
   bubble_sort_vec(v1,10);
   printf("v1: ");
   print_vec(v1,10);
   merge_sort_vec(v2,8);
   printf("v2: ");
   print_vec(v2,10);
   return 0;
}

