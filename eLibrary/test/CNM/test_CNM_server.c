
#include <CUnit/CUnit.h>

#include "common_types.h"
#include "common_queue.h"
#include "eLibrary_CNM_API.h"
#include "eLibrary_CNM_types.h"

COMMON_HANDLE f_conn_handle = 0U;

#define TEST_CNM_BOOK_FULL_ATTRIBUTE 0U

PUBLIC_BOOK_INFO f_book_array[] =
{
	{"book1", "This is book1", 0U, 0U, 1U, 2U, 3U},
	{"book1", "This is book1", 0U, 0U, 1U, 2U, 3U},
	{"book1", "This is book1", 0U, 0U, 1U, 2U, 3U}
};

PUBLIC_USER_INFO f_user_info;
sem_t f_test_CNM_server;

static void f_test_CNM_serverevent_listener(COMMON_HANDLE conn_handle, RETRIEVAL_TYPE retrieval_type, void *pay_load)
{
	if (true != g_negative_testing)
	{
		CU_ASSERT_EQUAL(COMMON_NULL_HANDLE, conn_handle);
	}
	else
	{
		SERVER_RESPONSE response_data;

		CU_ASSERT_NOT_EQUAL(COMMON_NULL_HANDLE, conn_handle);

        response_data.retrieval_type = retrieval_type;
        f_test_CNM_retrieval_type = retrieval_type;

        switch (retrieval_type)
		{
            case RETRIEVAL_TYPE_CREATE_USER:
				response_data.respond.response_status = COMMON_STATUS_OK;

				eLibrary_CNM_sendData(conn_handle, &response_data);
			case RETRIEVAL_TYPE_BROWSE_BOOKS:
				response_data.respond.content.num_objects = 1U;
				response_data.respond.content.object.books = &f_book_array[TEST_CNM_BOOK_FULL_ATTRIBUTE];

				eLibrary_CNM_sendData(conn_handle, &response_data);
#if 0
            case RETRIEVAL_TYPE_BROWSE_USERS:
				response_data.respond.content.num_objects = 1U;
				response_data.respond.content.object.users = &f_user_info;

				eLibrary_CNM_sendData(conn_handle, &response_data);
            case RETRIEVAL_TYPE_ADD_BOOK_DESP:
				response_data.respond.response_status = COMMON_STATUS_OK;
				response_data.respond.content.num_objects = 1U; response_data.respond.content.object.books = &f_book_array[TEST_CNM_BOOK_FULL_ATTRIBUTE]; eLibrary_CNM_sendData(conn_handle, &response_data);
            case RETRIEVAL_TYPE_ADD_FRIEND:
				response_data.respond.response_status = COMMON_STATUS_OK;
				response_data.respond.content.num_objects = 1U;
				response_data.respond.content.object.books = &f_book_array[TEST_CNM_BOOK_FULL_ATTRIBUTE];

				eLibrary_CNM_sendData(conn_handle, &response_data);
            case RETRIEVAL_TYPE_RM_FRIEND:
				response_data.retrieval_type = RETRIEVAL_TYPE_BROWSE_BOOKS;
				response_data.respond.response_status = COMMON_STATUS_OK;
				response_data.respond.content.num_objects = 1U;
				response_data.respond.content.object.books = &f_book_array[TEST_CNM_BOOK_FULL_ATTRIBUTE];

				eLibrary_CNM_sendData(conn_handle, &response_data);
#endif
			default:
			CU_DIAG_FAIL("Unhandled retrieval type %d.", retrieval_type);
		}

        sem_post(f_test_CNM_server);
	}

}

test_sem_wait(sem_t *sem)
{

}

void test_CNM_SHM_server(void)
{
	eLibrary_CNM_init();
	//eLibrary_CNM_addlugIn();

    result = eLibrary_CNM_registerEventCB(f_test_CNM_serverevent_listener);
   
    /* 1st call from client, create user account. */ 
    test_wait_sem(f_test_CNM_server, 3U); 
    CU_ASSERT_EQUAL(RETRIEVAL_TYPE_CREATE_USE, Rf_test_CNM_retrieval_type);
 
    /* 2rd call, browse a book. */
    test_wait_sem(f_test_CNM_server, 3U); 
    CU_ASSERT_EQUAL(RETRIEVAL_TYPE_CREATE_USER, f_test_CNM_retrieval_type);
    
    eLibrary_CNM_term();
}

void test_CNM_server_SHM()
{

}
CU_TestInfo test_CNM_server_tests[] = {
  { "server test", test_CNM_SHM_server},
  CU_TEST_INFO_NULL,
};






void suite_init_func()
{
	g_negative_testing = false;
}

void suite_clean_func()
{
	;
}
