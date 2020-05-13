#include <stdio.h>
#include <string.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

int lcs(char *str1, int len1, char *str2, int len2)
{
   int lcs[len1+1][len2+1];
   int i, j;

   memset(lcs, 0, sizeof(int)*(len1+1)*(len2+1));

   for (i=1; i<=len1; i++) {
      for (j=1; j<=len2; j++) {
         if (str1[i-1] == str2[j-1]) {
             lcs[i][j] = 1 + lcs[i-1][j-1];
         } else {
             lcs[i][j] = max(lcs[i-1][j], lcs[i][j-1]);
         }
      }
   }

   return lcs[len1][len2];
}

int main(void)
{
   char *str1 = "ABCDE";
   char *str2 = "ACE";
   int lcs_val = lcs(str1, strlen(str1), str2, strlen(str2));
   printf("LCS between %s and %s is %d\n", str1, str2, lcs_val);

   return 0;
}
