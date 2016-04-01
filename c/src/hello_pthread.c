#include <stdio.h>
#include <pthread.h>

void *hello( void *id )
{
    printf("%d: Hello World!\n", *((int*)id));
    return 0;
}

int main(int argc, char *argv[])
{
    const int COUNT = 5;
    int i;
    pthread_t thread[COUNT];
    int ids[COUNT];

    for (i=0;i<COUNT;i++)
    {
	ids[i]=i;
	int retval = pthread_create(&thread[i],NULL,hello,&ids[i]);
	if (retval)
	{
	    perror("pthread_create failed\n");
	    return 1;
	}
    }

    for (i=0;i<COUNT;i++)
	pthread_join(thread[i],NULL);

    return 0;
}
