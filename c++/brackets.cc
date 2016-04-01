#include <iostream>
#include <string>

using namespace std;

struct Brackets
{
    void print_brackets(string output, int pairs, int open, int close)
    {
	if ((open == close) && (open == pairs))
	{
	    cout << output << endl;
	}
	else
	{
	    if (open < pairs)
	        print_brackets(output + "(", pairs, open+1, close);

	    if (open > close)
	        print_brackets(output + ")", pairs, open, close+1);
        }
    }
};

int main(int argc, char *argv[])
{
    Brackets brackets;
    
    int pairs = 3;

    if (argc == 2)
	pairs = *argv[1]-'0';

    for (int i=1; i<= pairs; i++)
    {
	brackets.print_brackets("",i,0,0);
    }

    return 0;
}
