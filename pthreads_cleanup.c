/*
 * filename: pthreads_cleanup.c
 *
 * description: A code demonstrating the use of a cleanup handler
 *              This code also demonstrates the usage of the following API's
 *                pthread_cleanup_push()
 *                pthread_cleanup_pop()
 *                pthread_mutex_lock()
 *                pthread_mutex_unlock()
 *                pthread_cond_signal()
 *                pthread_cond_wait()
 *                pthread_create()
 *                pthread_cancel()
 *                pthread_join()
 *
 * date: Sept 23 2018
 *
 * To compile:
 * gcc pthreads_cleanup.c -o cleanup -lpthread -DDEBUG=1
 *
 * To execute:
 * ./cleanup
 * ./cleanup 1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "CommonHeader.h"


static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int gFlag = 0;

/*==============================================================================
 *  cleanupFunc
 *=============================================================================*/
static void cleanupFunc(void *arg)
{

    /* function to free memory and unlock mutex */

    int status;
    DLOG(C_VERBOSE,"deallocating memory at %p\n", arg);
    free(arg);
    DLOG(C_VERBOSE,"unlocking mutex\n");

    status = pthread_mutex_unlock(&mtx);
    if (status != 0){
        DLOG(C_ERROR,"Error: pthread_mutex_unlock. Status = %d\n",status);
    }
}

/*==============================================================================
 *  threadFunc
 *=============================================================================*/
static void * threadFunc(void *arg)
{
    int status;
    void *buf = NULL;

    /* allocate buffer */
    buf = (void *) malloc(0x10000);
    DLOG(C_VERBOSE,"allocated memory at %p\n",buf);

    status = pthread_mutex_lock(&mtx);
    if (status != 0){
        DLOG(C_ERROR,"pthread_mutex_lock. Status = %d\n",status);
    }
    pthread_cleanup_push(cleanupFunc, buf);

    while (gFlag == 0) {
        /* use pthread_cond_wait as a cancellation point */
        status = pthread_cond_wait(&cond, &mtx);

        if (status != 0){
            DLOG(C_ERROR,"pthread_cond_wait. Status = %d\n",status);
        }
        DLOG(C_VERBOSE,"condition wait loop completed\n");
    }

    DLOG(C_VERBOSE,"Calling pthread_cleanup_pop\n");

    /* execute cleanup function through pthread_cleanup_pop()*/
    pthread_cleanup_pop(1);

    DLOG(C_VERBOSE,"Exit\n");
    return NULL;
}

/*==============================================================================
 *  main
 *=============================================================================*/
int main(int argc, char *argv[])
{
    pthread_t thr;
    void *threadStatus;
    int status;

    status = pthread_create(&thr, NULL, threadFunc, NULL);
    if (status != 0){
        DLOG(C_VERBOSE,"pthread_create status = %d\n",status);
    }

    sleep(2);

    if (argc == 1) {
        DLOG(C_VERBOSE,"Calling pthread_cancel\n");

        status = pthread_cancel(thr);
        if (status != 0){
            DLOG(C_VERBOSE,"pthread_cancel status=%d\n",status);
        }
    } else {

        gFlag = 1;
        DLOG(C_VERBOSE,"Calling pthread_cond_signal\n");
        status = pthread_cond_signal(&cond);
        if (status != 0){
            DLOG(C_VERBOSE, "pthread_cond_signal status = %d\n",status);
        }
    }
    status = pthread_join(thr, &threadStatus);
    if (status != 0){
        DLOG(C_VERBOSE,"pthread_join status = %d\n",status);
    }
    if (threadStatus == PTHREAD_CANCELED){
        DLOG(C_VERBOSE,"thread has been canceled\n");
    }
    else{
        DLOG(C_VERBOSE,"thread has terminated\n");
    }
    exit(0);
}
