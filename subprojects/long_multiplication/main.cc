#include <iostream>
#include "wideint.h"
typedef wideint<256> int256_t;
typedef wideint<128> int128_t;

int main(void)
{
    int256_t a = 16;
    int256_t b = 8;

    auto c = a*b;

    std::cout << "a = " << a.to_string(2) << std::endl;
    std::cout << "b = " << b.to_string(2) << std::endl;
    std::cout << "c = a*b = " << c.to_string(2) << std::endl;

    int128_t e = 0b11011;
    int128_t f = 0b1011;

    auto g = e % f;
    auto h = e / f;
    auto i = h*f + g;

    std::cout << "e = " << e.to_string(2) << std::endl;
    std::cout << "f = " << f.to_string(2) << std::endl;
    std::cout << "g = e%f = " << g.to_string(2) << std::endl;
    std::cout << "h = e/f = " << h.to_string(2) << std::endl;
    std::cout << "i = (e/f)*f + e%f = " << i.to_string(2) << std::endl;

    return 0;
}
