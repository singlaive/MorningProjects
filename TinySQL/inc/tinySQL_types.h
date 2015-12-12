/*
 * tinySQL_types.h
 *
 *  Created on: 10 Feb 2014
 *      Author: murphy
 */

#ifndef TINYSQL_TYPES_H_
#define TINYSQL_TYPES_H_

#define TINYSQL_VALUE_INVALID 0U

typedef enum
{
	tinySQL_DATA_TYPE_invalid,
	tinySQL_DATA_TYPE_UINT32,
	tinySQL_DATA_TYPE_TEXT,
	tinySQL_DATA_TYPE_BOOL,
	tinySQL_DATA_TYPE_DATE,
	tinySQL_DATA_TYPE_BLOB,
	tinySQL_DATA_TYPE_lastvalue
}tinySQL_DATA_TYPE;

#endif /* TINYSQL_TYPES_H_ */
