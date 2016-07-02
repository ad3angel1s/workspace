#include <iostream>

using namespace std;

int main()
{
  int a[] = {1, 2, 3, 4};

  for (auto &i : a)
    cout << i << endl;

  return 0;
}
