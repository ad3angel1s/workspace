#include "systemc.h"

SC_MODULE(hello_world)
{
    SC_CTOR(hello_world)
    {
	// Nothing
    }

    void say_hello()
    {
	cout << "\nHello, world!" << endl;
    }
};

int sc_main(int argc, char *argv[])
{
    hello_world hello("HELLO");

    hello.say_hello();

    return 0;
}
