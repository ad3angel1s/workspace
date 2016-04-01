#include <vector>
#include <iostream>

int main()
{
    std::vector<int> test_vector{1,2,3,4};

    test_vector.push_back(5);

    // auto, deduces the type automatically
    // range for ii is of single type of the collection test_vector
    for (const auto ii: test_vector)
    {
		std::cout << ii << std::endl;
    }

    return 0;
}
