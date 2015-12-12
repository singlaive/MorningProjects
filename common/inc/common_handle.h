/*
 * common_handle.h
 *
 *  Created on: 12 Aug 2013
 *      Author: singlaive
 */

#ifndef COMMON_HANDLE_H_
#define COMMON_HANDLE_H_


SYSTEM_STATUS COMMON_HDL_handleCreate(const void *p_data, COMMON_HANDLE *p_handle);

SYSTEM_STATUS COMMON_HDL_handleDestroy(COMMON_HANDLE handle);

SYSTEM_STATUS COMMON_HDL_getObject(COMMON_HANDLE handle, void **p_data);

#endif /* COMMON_HANDLE_H_ */
