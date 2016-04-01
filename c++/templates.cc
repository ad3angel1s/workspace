#include <cstdio>

template<typename T>
struct traits
{
  static const bool property = true;
};

template<>
struct traits<int>
{
  static const bool property = false;
};

template<typename T>
void property_enabled(T val)
{
  /*
  traits<T> trait_instance;
  
  if (trait_instance.property)
  */
  if (traits<T>::property)
  {
    printf("Property is enabled\n");  
  }
  else
  {
    printf("Property is not enabled\n");
  }
  
}

int main()
{
  property_enabled(3);

  property_enabled(3.0);
}
  

  
