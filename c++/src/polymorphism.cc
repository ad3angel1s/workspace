#include <iostream>
using namespace std;

/*
   vptr is a field of each class which points to the vtable associated
   to every class. The vtable is constructed when there is a virtual function
   in the base class. The vtable contains the addresses of each of the virtual
   functions for that class (i.e. in this case, for B()), so that it will point
   to the correct method to run based on the object instatiated.

   ndr: a copy constructor is like a constructor but with const T& as a parameter
*/

struct Base
{
    void A()
	{
	    cout << "This is base A" << endl;
	};

    virtual void B()
	{
	    cout << "This is base B" << endl;
	}
};

struct Child: public Base
{
    void A()
	{
	    cout << "This is child A" << endl;
	}

    void B()
	{
	    cout << "This is child B" << endl;
	}
};

int main()
{
    Base base;

    Child child;

    Base *pBase = &child;

    base.A();
    base.B();
    child.A();
    child.B();

    pBase->A();
    pBase->B();
    
    return 0;
}
    
