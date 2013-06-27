/*
 * test_eLibrary.c
 *
 *  Created on: 14 Nov 2012
 *      Author: singlaive
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include "test_eLibrary.h"
#include "eLibrary_serverHandler.h"

void test_serverInit(void);
void test_serverTerm(void);

int main(int argc, char **argv)
{
	CU_pSuite pSuite = NULL;


	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	pSuite = CU_add_suite("suite_serverInit", NULL, NULL);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (NULL == CU_add_test(pSuite, "TEST: Server Initialization", test_serverInit))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	pSuite = CU_add_suite("suite_serverHandlers", NULL, NULL);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(pSuite, "TEST: Server Handlers - Add users", test_serverHandler_userAdd)) ||
		(NULL == CU_add_test(pSuite, "TEST: Server Handlers - Add books", test_serverHandler_bookAdd)) ||
		(NULL == CU_add_test(pSuite, "TEST: Server Handlers - Retrieve book properties", test_serverHandler_bookProp)) ||
		(NULL == CU_add_test(pSuite, "TEST: Server Handlers - Create user activities and book history", test_serverHandler_createActy)) ||
		(NULL == CU_add_test(pSuite, "TEST: Server Handlers - Send/receive messages", test_serverHandler_msgs)) ||
		(NULL == CU_add_test(pSuite, "TEST: Server Handlers - Retrieve user activies and bok history", test_serverHandler_checkActies)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	pSuite = CU_add_suite("suite_serverTerm", NULL, NULL);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (NULL == CU_add_test(pSuite, "TEST: Server Termination", test_serverTerm))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	/*CU_basic_run_tests();*/
	CU_automated_run_tests();
	CU_list_tests_to_file();

	printf("\n\n");
	printf("======================================\n");
	printf("    number of suites run: %d\n", CU_get_number_of_suites_run());
	printf("	number of suites failed: %d\n", CU_get_number_of_suites_failed());
	printf("	number of tests run: %d\n", CU_get_number_of_tests_run());
	printf("	number of tests failed: %d\n", CU_get_number_of_tests_failed());
	printf("	number of asserts done: %d\n", CU_get_number_of_asserts());
	printf("	number of asserts successful: %d\n", CU_get_number_of_successes());
	printf("	number of asserts failed: %d\n", CU_get_number_of_failures());
	printf("======================================\n");


	CU_cleanup_registry();
	return CU_get_error();
}

void test_serverInit(void)
{
	SYSTEM_STATUS result = STATUS_FAIL;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Server initialization.");
	result = eLibrary_serverInit();
	CU_ASSERT_EQUAL(result, STATUS_OK);
	printf(">>>>>>>>>>>>>>>>>\n");
}

void test_serverTerm(void)
{
	SYSTEM_STATUS result = STATUS_FAIL;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Server Termination.");
	result = eLibrary_serverTerm();
	CU_ASSERT_EQUAL(result, STATUS_OK);
	printf(">>>>>>>>>>>>>>>>>\n");
}
