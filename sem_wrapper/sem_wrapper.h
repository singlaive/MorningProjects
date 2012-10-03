/*
 * sem_wrapper.h
 *
 *  Created on: 2012-3-12
 *      Author: singlaive
 */

#ifndef SEM_WRAPPER_H_
#define SEM_WRAPPER_H_

#define sem_wrapper_t pthread_cond_t

int sem_wrapper_init(sem_wrapper_t *sem, int pshared, unsigned value);
int sem_wrapper_destroy(sem_wrapper_t *sem);
int sem_wrapper_wait(sem_wrapper_t *sem);
int sem_wrapper_trywait(sem_wrapper_t *sem);
int sem_wrapper_timedwait(sem_wrapper_t * sem, const struct timespec *abs_timeout);
int sem_wrapper_post(sem_wrapper_t *sem);
int sem_wrapper_getvalue(sem_wrapper_t * sem, int *sval);

#endif /* SEM_WRAPPER_H_ */
