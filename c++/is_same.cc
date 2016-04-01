#include <iostream>

using namespace std;

template<typename T>
struct add_const_ref
{
    typedef const T& type;
};

template<typename T>
struct add_const_ref<T&>
{
    typedef T type;
};

template<typename T, typename U>
struct is_same_type
{
    static const bool value = false;
};

template<typename T>
struct is_same_type<T,T>
{
    static const bool value = true;
};

int main()
{
    const int a = 3;
    
    add_const_ref<int>::type b = a;
    
    cout << "a: "<< a << " b: " << b << endl;

    cout << "result: " << is_same_type<const int&, add_const_ref<int>::type >::value << endl;

    cout << "result: " << is_same_type<float, add_const_ref<int>::type >::value << endl;

    cout << "result: " << is_same_type<float, float>::value << endl;
    
    return 0;
}
