#include <iostream>
using namespace std;

void printArray(int v[], int sz)
{
    for (int i=0; i<sz; i++)
	cout << v[i] << " ";
    cout << endl;
}

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

void insertionSort(int v[], int sz)
{
    /* using a support array */
    bool bInPlace = true;

    if (bInPlace)
    {
        int u[sz];
        int usz = 0;

        for (int i=0; i<sz; i++)
	    u[i] = 0;

        for (int i=0; i<sz; i++)
        {
	    for (int j=0; j<sz; j++)
	    {
 	        //printArray(u, sz);
	    
	        //cout << v[i] << u[j] << usz << endl;
	    
	        if (j == usz)
	        {
		    u[j] = v[i];
		    usz++;
		    break;
	        }
	        else if (u[j] > v[i])
	        {
		    for (int k=usz-1; k>=j; k--)
		        u[k+1] = u[k];

		    u[j] = v[i];
		    usz++;
		    break;
	        }
	    }
        }

        for (int i=0; i<sz; i++)
	    v[i] = u[i];

    }
}

int main()
{
    int v[] = {3, 4, 1, 5, 2, 4, 7};
    int sz  = 7;

    printArray(v,sz);
    
    insertionSort(v,sz);

    printArray(v,sz);

    return 0;
}
    
   
