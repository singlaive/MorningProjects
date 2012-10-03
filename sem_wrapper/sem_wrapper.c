/*
 * sem_wrapper.c
 *
 *  Created on: 2012-3-12
 *      Author: singlaive
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include "sem_wrapper.h"
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
unsigned int f_sem_value = 0;

int sem_wrapper_init(sem_wrapper_t *sem, int pshared, unsigned value)
{
	int status = -1;

	sem = (sem_wrapper_t*)&cond;

	if (-1 !=  pthread_mutex_lock(&mutex))
	{
        f_sem_value = value;

        if (-1 == pthread_mutex_unlock(&mutex))
        {
            printf("Error - pthread_mutex_unlock failed (%#x).", status);
        }
        else
        {
            status = 0;
        }
    }
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}

int sem_wrapper_destroy(sem_wrapper_t *sem)
{
	return 0;
}

int sem_wrapper_wait(sem_wrapper_t *sem)
{
	int status = 0;
	pthread_cond_t *l_cond = (pthread_cond_t *)sem;

	if (-1 !=  pthread_mutex_lock(&mutex))
    {
    	while(0 == f_sem_value)
    	{
    		if (0 != pthread_cond_wait(l_cond, &mutex))
            {
                printf("Error - pthread_cond_wait failed (%#x).", status);
                status = -1;
                break;
            }
    	}

        if (0 == status)
        {
    	    f_sem_value--;

            if (-1 == pthread_mutex_unlock(&mutex))
            {
                printf("Error - pthread_mutex_unlock failed (%#x).", status);
                status = -1;
            }
        }
    }
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}

int sem_wrapper_trywait(sem_wrapper_t *sem)
{
	int status = 0;
	pthread_cond_t *l_cond = (pthread_cond_t *)sem;
	struct timespec abs_timeout;

	abs_timeout.tv_sec = time(NULL) + 1;
	if (-1 !=  pthread_mutex_lock(&mutex))
	{
    	while(0 == f_sem_value)
    	{
            status = pthread_cond_timedwait(l_cond, &mutex, &abs_timeout);
     	    if ((ETIMEDOUT == status) && (0 == f_sem_value))
     	    {
                printf("Error - pthread_cond_wait timedout (%#x).", status);
                status = EAGAIN;
                break;
            }
            else if (0 != status)
            {
                printf("Error - pthread_cond_wait failed (%#x).", status);
                status = -1;
                break;
            }
            else
            {
                /* Do nothing. */
            }
        }

        if (0 == status)
        {
        	f_sem_value--;

            if (-1 != pthread_mutex_unlock(&mutex))
            {
                printf("Error - pthread_mutex_unlock failed (%#x).", status);
                status = -1;
            }
        }
	}
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}

int sem_wrapper_timedwait(sem_wrapper_t *sem, const struct timespec *abs_timeout)
{
	int status = -1;
	pthread_cond_t *l_cond = (pthread_cond_t *)sem;

	if (-1 !=  pthread_mutex_lock(&mutex))
	{
    	while(0 == f_sem_value)
    	{
            status = pthread_cond_timedwait(l_cond, &mutex, abs_timeout);
     	    if ((ETIMEDOUT == status) && (0 == f_sem_value))
     	    {
                printf("Error - pthread_cond_wait timedout (%#x).", status);
                break;
            }
            else if (0 != status)
            {
                printf("Error - pthread_cond_wait failed (%#x).", status);
                status = -1;
                break;
            }
            else
            {
                /* Do nothing. */
            }
    	}

        if (0 == status)
        {
        	f_sem_value--;

            if (-1 != pthread_mutex_unlock(&mutex))
            {
                printf("Error - pthread_mutex_unlock failed (%#x).", status);
                status = -1;
            }
        }
    }
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}

int sem_wrapper_post(sem_wrapper_t *sem)
{
	int status = -1;
	pthread_cond_t *l_cond = (pthread_cond_t *)sem;

	if (-1 !=  pthread_mutex_lock(&mutex))
	{
        f_sem_value++;

    	if (-1 != pthread_cond_broadcast(l_cond))
    	{
            if (-1 == pthread_mutex_unlock(&mutex))
            {
                printf("Error - pthread_mutex_unlock failed (%#x).", status);
            }
            else
            {
                status = 0;
            }
        }
        else
        {
            printf("Error - pthread_cond_broadcast failed (%#x).", status);
        }
    }
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}

int sem_wrapper_getvalue(sem_wrapper_t *sem, int *sval)
{
	int status = -1;

	if (-1 !=  pthread_mutex_lock(&mutex))
    {
	    *sval = f_sem_value;

        if (-1 == pthread_mutex_unlock(&mutex))
        {
            printf("Error - pthread_mutex_unlock failed (%#x).", status);
        }
        else
        {
            status = 0;
        }
    }
    else
    {
        printf("Error - pthread_mutex_lock failed (%#x).", status);
    }

	return status;
}
