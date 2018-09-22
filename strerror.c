/*
 * filename: strerror.c
 * testfile: test_strerror.c
 * description: A simple thread safe implementation of strerror(),
 *              using thread specific data.
 *              This code along with test_strerror.c,  also demonstrates the usage of the following API's
 *                pthread_getname_np()
 *                pthread_setname_np()
 *                pthread_once()
 *                pthread_key_create()
 *                pthread_getspecific()
 *                pthread_setspecific()
 *                pthread_join()
 *                pthread_create()
 * date: sept 21 2018
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <pthread.h>


extern pthread_t t;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

#define MAX_ERROR_LEN 256

/* Free thread-specific data buffer using destructor */
static void destructor(void *buf)
{
    free(buf);
}

/* One-time key creation function */
static void  keyCreate(void)
{
    int status;
    printf("Calling  pthread_key_create\n");
    status = pthread_key_create(&strerrorKey, destructor);
    if (status != 0)
        printf("pthread_key_create error\n");
}


char * strerror(int err)
{
    int status;
    char *buf;
    /* Make first caller allocate key for thread-specific data */
    status = pthread_once(&once, keyCreate);
    if (status != 0){
        printf("pthread_once error");
    }
    
    buf = pthread_getspecific(strerrorKey);
    if (buf == NULL) {
        /* If this is the first call from this thread then allocate
           buffer for this thread and save its location by calling pthread_setspecific()
           */
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL){
            printf("malloc failed");
        }
        status = pthread_setspecific(strerrorKey, buf);
        if (status != 0) {
            printf("pthread_setspecific failed");
        }
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }
    return buf;
}

