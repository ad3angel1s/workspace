#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct triangle
{
	int a;
	int b;
	int c;
};

typedef struct triangle triangle;
void sort_by_area(triangle* tr, int n) {
	double areas[n];
    int i=0, j=0;
    for (i=0; i<n; i++) {
        double p = ((double)tr[i].a + (double)tr[i].b + (double)tr[i].c)/2.0;
	double first = fabs(p-tr[i].a);
	double second = fabs(p-tr[i].b);
	double third = fabs(p-tr[i].c);
	//printf("p=%g,first=%d,second=%g,third=%g\n",p,first,second,third);
        areas[i] = sqrt(p * first * second * third);
        //printf("areas[%d]=%g\n",i,areas[i]);
    }
    for (i=0; i<n; i++) {
        for (j=i+1; j<n; j++) {
            if (areas[i] > areas[j]) {
                triangle tmp = tr[i];
                tr[i] = tr[j];
                tr[j] = tmp;
		double area = areas[i];
		areas[i] = areas[j];
		areas[j] = area;
            }
        }
    }
}

int main()
{
        FILE *fp = fopen("test.txt","rt");
	int n;
	fscanf(fp, "%d", &n);
	triangle *tr = malloc(n * sizeof(triangle));
	for (int i = 0; i < n; i++) {
		fscanf(fp, "%d%d%d", &tr[i].a, &tr[i].b, &tr[i].c);
	}
	sort_by_area(tr, n);
	for (int i = 0; i < n; i++) {
		printf("%d %d %d\n", tr[i].a, tr[i].b, tr[i].c);
	}
	return 0;
}
