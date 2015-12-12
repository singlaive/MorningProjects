/*
 * common_stack.h
 *
 *  Created on: 28 Aug 2013
 *      Author: singlaive
 */

#ifndef COMMON_STACK_H_
#define COMMON_STACK_H_

#include "common_types.h"
#include "common_handle.h"


SYSTEM_STATUS COMMON_STACK_create(uint32_t stack_length, STACK_HANDLE *p_stack_handle);

SYSTEM_STATUS COMMON_STACK_destroy(STACK_HANDLE stack_handle);

SYSTEM_STATUS COMMON_STACK_configure(STACK_HANDLE stack_handle, STACK_TYPE stack_type);

SYSTEM_STATUS COMMON_STACK_push(STACK_HANDLE stack_handle, void *data, size_t data_size, bool allocate_memory);

SYSTEM_STATUS COMMON_STACK_pop(STACK_HANDLE stack_handle, void *data);

SYSTEM_STATUS COMMON_STACK_getNext(STACK_HANDLE stack_handle, STACK_POSITION position);

#endif /* COMMON_STACK_H_ */
