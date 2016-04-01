#include <cstdio>
#include <cstdlib>

using namespace std;

struct List
{
    int el;
    List *pList;

    List();
    List(int el);
    ~List();
    void insert(int el);
    void remove();

    void print();
    List *reverse();

    void *operator new(size_t sz);
    void operator delete(void *ptr);
};
    
List::List()
{
    this->el = 0;
    this->pList = NULL;
}

List::List(int el)
{   
    this->el = el;
    this->pList = NULL;

    printf("0x%llx[%d] -> 0x%llx\n", (unsigned long long) this, this->el, (unsigned long long) this->pList);
}

List::~List()
{
    printf("Calling dtor on 0x%llx\n", (unsigned long long) this);
    if (this->pList)
	this->pList->remove();
}

void List::insert(int el)
{
    List *list = this;
    
    while (list->pList)
	list = list->pList;

    printf("Moved to 0x%llx... ", (unsigned long long) list);
  
    list->pList = new List;
    
    list->pList->el = el;
    list->pList->pList = NULL;

    printf("0x%llx[%d] -> 0x%llx\n", (unsigned long long) list->pList, list->pList->el, (unsigned long long) list->pList->pList);
}

void List::remove()
{   
    if (this->pList)
    {
	this->pList->remove();
	this->pList = NULL;
    }
    
    printf("Freeing 0x%llx\n", this);
    delete this; // delete will call the dtor but we have set it to NULL so just print
    	    
}

void List::print()
{
    printf("0x%llx[%d] -> 0x%llx\n", (unsigned long long) this, this->el, (unsigned long long) this->pList);
    if (this->pList)
	this->pList->print();
}

void *List::operator new(size_t sz)
{
    return malloc(sz);
}

void List::operator delete(void *ptr)
{
    free(ptr);
    return;
}

List *List::reverse()
{
    List *prev = NULL, *curr = this, *store = NULL;

    /*
       Algorithm: NULL -> A -> B -> C -> NULL

       init->   prev: NULL, curr: A, store: NULL

       repeat-> prev: A, curr: B, prev->next: store, store: A

       while->  curr != NULL
    */

    do
    {
        prev = curr;
        curr = curr->pList;

        prev->pList = store;
	store = prev;

	printf("Store 0x%llx[%d]\n", (unsigned long long) store, store->el);
    }
    while (curr);
    
    return store;
}

int main(int argc, char *argv[])
{
    printf("\n");
    
    if (argc == 1)
    {
	printf("Not enough arguments\n");
	return 1;
    }
    else if (argc == 2)
    {
	printf("Length is %d but no elements are provided\n", atoi(argv[1]));
	return 2;
    }
    else if ((argc > 2) &&(atoi(argv[1]) <= 0))
    {
	printf("Length is %d but it must be a positive non zero number\n", atoi(argv[1]));
	return 3;
    }
    else
    {
	printf("Inputs are: ");
	for (int idx=1; idx<argc; idx++)
	    printf("%d ", atoi(argv[idx]));

	printf("\n\n");
    }	

    List list( atoi(argv[2]) ), *pList = &list;
    for (int idx=1; idx < atoi(argv[1]); idx++)
    {
        list.insert(atoi(argv[idx+2]));
    }

    printf("\n");

    list.print();
    
    printf("\n");

    pList = list.reverse();

    printf("\n");
    
    pList->print();

    printf("\n");

    pList = pList->reverse();

    printf("\n");

    pList->print();

    printf("\n");

    return 0;
}
	

    
	
