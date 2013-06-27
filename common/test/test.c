/*
 * test.c
 *
 *  Created on: 15 Oct 2012
 *      Author: singlaive
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "CUnit/Basic.h"
#include "CUnit/Automated.h"

uint32_t i = 0U;
void testINFO(void);
void testFATAL(void);

int main(int argc, char **argv)
{
	CU_pSuite pSuite = NULL;

	printf("This must be shown!\n");

	if (argc > 1)
	{
		i = atoi(argv[1]);
		printf("i = %d", i);
	}

	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	pSuite = CU_add_suite("Suite_1", NULL, NULL);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(pSuite, "test of INFO", testINFO)) ||
		(NULL == CU_add_test(pSuite, "test of FATAL", testFATAL)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_automated_run_tests();
	CU_list_tests_to_file();

	CU_cleanup_registry();
	return CU_get_error();
}

void testINFO(void)
{
	DIAG_INFO("DIAG macro test.");
	time_t sec = 55555U;
	printf("Meng: the current date is %s\n", myAscTimeLite(&sec));
	if (i == 0)
	{
		CU_FAIL("It failed!");
	}
	else
	{
		CU_PASS("It passed!");
	}

}

void testFATAL(void)
{
	char errStr[10U];
	errno = EPERM;

	DIAG_FATAL("This should be a FATAL.");
	CU_ASSERT_EQUAL(1, i);
}
