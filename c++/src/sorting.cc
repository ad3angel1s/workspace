#include <iostream>
using namespace std;

/* 
 *  Bubble sort:    compare each element and bubble it up
 *  Selection sort: find the minimum and put at the end, work recursively on the remaining array
 *  Insertion sort: pick an element and insert it in the ordered array, which is empty at the beginning
 *  Quick sort:     choose the pivot and put all elements in place -> O(nlog(n) on average)
 *  Merge sort:     divide and conquer (O(nlog(n)))
 */

void bubbleSort(int v[], int sz)
{
    int iter = sz;
    
    for (int j=0; j<iter; j++)
    {
        for (int i=0; i<sz-1; i++)
        {
	    if (v[i] >= v[i+1])
	    {
	        int tmp = v[i];
	        v[i] = v[i+1];
	        v[i+1] = tmp;
	    }
        }

	/* Each iteration doesn't have to go up until the last element */
	sz-=1;
    }
}

void printArray(int v[], int sz)
{
    for (int i=0; i<sz; i++)
	cout << v[i] << " ";
    cout << endl;
}

int main()
{
    int v[] = {3, 4, 1, 5, 2, 4, 7};
    int sz  = 7;

    printArray(v,sz);

    bubbleSort(v,sz);

    printArray(v,sz);

    return 0;
}
    
   
