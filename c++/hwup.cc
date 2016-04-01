#include<iostream>
#include<string>
using namespace std;
int main()
{
	int i;
	int array[5];
	for(i=0;i<5;i++)
	{
		cin>>array[i];
		
		while(array[i]<0 || array[i]>10)
		{
			cin>>array[i];
		}
        }
    return 0;
}
