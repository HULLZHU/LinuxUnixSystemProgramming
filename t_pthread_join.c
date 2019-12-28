#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
void* func(void *arg)
{
    char *s = (char *)arg;

    printf("Thread %ld: %s\n", pthread_self(), s);

    return (void*)strlen(s);                                    /*Cast back to the void* type */

}

int main(int argc, char *argv[])
{
    pthread_t t1;
    void *result;
    
    int s = pthread_create(&t1, NULL, func, "Hello HeZhu\n");   /*create a pthread*/

    if(s != 0 )
    {
        printf("Err: pthread_create err: %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("main(): \n");

    s = pthread_join(t1, &result);
    if( s != 0 )
    {
        printf("Err: pthread_join err: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("Thread returned %ld\n", (long)result);

    pthread_exit(NULL);
}