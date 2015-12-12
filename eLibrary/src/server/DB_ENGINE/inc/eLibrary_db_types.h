/*
 * eLibrary_db_types.h
 *
 *  Created on: 5 Feb 2014
 *      Author: murphy
 */

#ifndef ELIBRARY_DB_TYPES_H_
#define ELIBRARY_DB_TYPES_H_

typedef struct
{

}DB_SEARCH_CRITERIA;

typedef struct
{
	TREE_NODE *root;
	uint32_t numBooks;
	uint32_t actyHeadOffset;		/* Pointing to the start point in the activity array. */
	uint32_t actyTailOffset;		/* Pointing to the next available slot for new entry. */
	char activities[MAX_ACTY_LENGTH][MAX_ACTY_LENGTH]; /* This is a ring array to store recently activities. Thus the beginning of the array might be overiden by later entries. */
}BOOK_LIBRARY;

#endif /* ELIBRARY_DB_TYPES_H_ */
