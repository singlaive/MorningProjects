/*
 * eLibrary_shm.h
 *
 *  Created on: 1 Sep 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_SHM_H_
#define ELIBRARY_SHM_H_

#define MAX_SHM_PATH_LENGTH 99U

typedef struct
{
	char shm_path[MAX_SHM_PATH_LENGTH];
}CONN_ADDR;

SYSTEM_STATUS eLibrary_SHM_init(CONN_ADDR *p_conn_addr);
SYSTEM_STATUS eLibrary_SHM_term(CONN_ADDR *p_conn_addr);
SYSTEM_STATUS eLibrary_SHM_sendData(CONN_ADDR *p_conn_addr, void *data, uint32_t data_length);
SYSTEM_STATUS eLibrary_SHM_getData(CONN_ADDR *p_conn_addr, uint32_t *data_length, void *data);

#endif /* ELIBRARY_SHM_H_ */
