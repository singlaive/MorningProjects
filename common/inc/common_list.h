/*
 * common_list.h
 *
 *  Created on: 23 Sep 2013
 *      Author: singlaive
 */

#ifndef COMMON_LIST_H_
#define COMMON_LIST_H_

int (*elementFind_func)(void *list_element, void*private_data);

COMMON_LIST_processElement();
COMMON_LIST_getHead();
COMMON_LIST_getTail();
COMMON_LIST_getCount();
/* At the end */
COMMON_LIST_attachElement();
/* At the beginning */
COMMON_LIST_insertElement();
COMMON_LIST_findElement(LIST_HANDLE list_handle, elementFind_func find_func, void *private_data, void *found_element);



#endif /* COMMON_LIST_H_ */
