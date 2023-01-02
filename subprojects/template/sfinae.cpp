#include <iostream>

template <typename T, typename U>
void func(void) {
  std::cout << "Generic Template" << std::endl;
}

template <typename T, std::size_t N = 0>
void func(void) {
  std::cout << "Specialised template " << N <<  std::endl;
}

int main() {

  /* This matches to the generic template */
  func<int,int>();

  /* This matches to the specialised template, as the second template parameter is defaulted */
  func<int>();

  /* This SFINAE on the specialised template and resolves back to the generic template */
  func<int, std::size_t>();

  /* This matches the specialised template again */
  func<int, 5>();
  
  return 0;
}
