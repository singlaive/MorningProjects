/*
 ============================================================================
 Name        : test_semamphore.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include "sem_wrapper.h"

sem_wrapper_t sem;

void * thread_routine(void * arg);

int main(void)
{
	int status = EXIT_FAILURE;
	int int_value = -1;
    pthread_t thread_id;

	sem_wrapper_init(&sem, 0, 0);
	sem_wrapper_getvalue(&sem, &int_value);
	assert(int_value == 0);
	printf("sem value %d\n", int_value);

    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if (EXIT_SUCCESS != status)
    {
        printf("Error - Fail to create thread.\n");
    }

    printf("Info - Waiting for sem...\n");
    sem_wrapper_wait(&sem);

	sem_wrapper_destroy(&sem);

	return EXIT_SUCCESS;
}

void * thread_routine(void * arg)
{

	printf("Info - routine starts.\n");
	printf("Info - Sleep for 3sec before posting sem...\n");
	sleep(3);
	sem_wrapper_post(&sem);
	printf("Info - sem posted.\n");

	printf("Info - routine ends.\n");

	return NULL;
}
