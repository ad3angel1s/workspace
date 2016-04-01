#include <iostream>

// define the tags, i.e. empty structs which define a metadata
struct fast_speed_tag {};
struct slow_speed_tag {};

// default
template <typename T>
struct traits
{
    typedef slow_speed_tag speed;
};

// fast specialisation for int (a partial specialisation doesn't need the typename)
template <>
struct traits<int>
{
    typedef fast_speed_tag speed;
};

// define the overloaded functions for each tag
template <typename T>
void work_dispatch( const T &val, const slow_speed_tag& )
{
    std::cout << "Slow" << std::endl;
}

template <typename T>
void work_dispatch( const T &val, const fast_speed_tag& )
{
    std::cout << "Fast" << std::endl;
}

// real function which acts based on the tag associated to the type param
template <typename T>
void work( const T &val)
{
    work_dispatch<T>( val, typename traits<T>::speed() );
    // the second argument calls the default constructor for the dependent qualified type
    // here the type T could be passed explicitly like this or not, in that case it's
    // deducted
}

int main()
{
    int a;
    float b;
    long int c;

    work(a);
    work(b);
    work(c);

    return 0;
}
