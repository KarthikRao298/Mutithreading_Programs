/*
 * filename: test_strerror.c
 * description: This file tests the thread safe implementation of strerror() 
 * .            implemented in strerror.c
 * 
 * date: sept 21 2018
 * To compile:
 * gcc strerror.c test_strerror.c -lpthread -o test.exe
 *
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>  

#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 

#define MAX_THREAD_NAME_LEN 16
typedef enum { FALSE, TRUE } Boolean;

pthread_t t;

static void * threadFunc(void *arg)
{

    char threadName[MAX_THREAD_NAME_LEN];


    int status = pthread_getname_np(t, threadName,MAX_THREAD_NAME_LEN);
    if (status!=0){
        printf("pthread_getname_np failed. status =%d\n",status);
    }

    char *str;
    str = strerror(EPERM);
    printf("%s thread has called strerror()\n", threadName);
    printf("%s thread: str (%p) = %s\n",threadName, str, str);
    return NULL;
}

int main(int argc, char *argv[])
{

    int status;
    char *str;
    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");
    status = pthread_create(&t, NULL, threadFunc, NULL);

    pthread_setname_np(t, "child");


    if (status != 0)
        printf("pthread_create error");
    status = pthread_join(t, NULL);
    if (status != 0)
        printf("pthread_join error");
    printf("Main thread: str (%p) = %s\n", str, str);

    pthread_exit(NULL);
}

