

#include <CUnit/CUnit.h>

#include "common_types.h"
#include "eLibrary_CNM_API.h"
#include "eLibrary_CNM_types.h"


COMMON_HANLDE f_test_CNM_CLT_returned_request;
SERVER_RESPONSE *f_test_CNM_CLT_response;

sem_t f_test_CNM_CLT;

static void f_test_CNM_responseListener(COMMON_HANDLE request_handle, 
                                        SERVER_RESPONSE *p_response)
{
	RETRIEVAL_TYPE retreival_type;
	COMMON_HANDLE user_id;

	if (true != g_negative_testing)
	{
		CU_ASSERT_EQUAL(COMMON_NULL_HANDLE, conn_handle);
	}
	else
	{
        f_test_CNM_CLT_returned_request = request_handle;
        f_test_CNM_CLT_response = p_response;
	}

    sem_post(&f_test_CNM_CLT);
}

void test_CNM_SHM_client(void)
{
	COMMON_HANDLE client_id = 0U;
    COMMON_HANDLE ipc_queue_handle;
    uint32_t payload_size;
    void *pay_load = NULL;
    CLIENT_REQUEST request_data;
    COMMON_HANDLE request_handle;

	result = eLibrary_CNM_CLT_init();
    result = eLibrary_CNM_CLT_setIdentity("Mia", "Mia's password");
    result = eLibrary_CNM_CLT_register_client(&client_id);
    CU_ASSERT_NOT_EQUAL(0U, client_id);

    result = eLibrary_CNM_CLT_registerResponseListener(f_test_CNM_responseListener);

    request_data.retrieval_type = RETRIEVAL_TYPE_BROWSE_BOOKS;
    request_data.sort_id = SORT_INVALID;
    request_data.filter_id = FILTER_BY_BOOK_NAME;
    request_data.object_str = "Nancy";    
    result = eLibrary_CNM_CLT_createRequest(request_data, &request_handle);
    result = eLibrary_CNM_CLT_sendRequest(request_handle); 
    test_wait_sem(f_test_CNM_CLT);
    CU_ASSERT_EQUAL(request_handle, f_test_CNM_CLT_returned_request); 
    if (NULL != f_test_CNM_CLT_response)
    {
        CU_ASSERT_EQUAL(RETRIEVAL_TYPE_BROWSE_BOOKS, f_test_CNM_CLT_response->retrieval_type);
        CU_ASSERT_EQUAL(1U, f_test_CNM_CLT_response->respond.content.num_objects);
        CU_ASSERT_EQUAL(NULL, f_test_CNM_CLT_response->respond.content.object.books);
        if (NULL != f_test_CNM_CLT_response->respond.content.object.books)
        {
            CU_ASSERT_EQUAL(0, strcpy("book1", f_test_CNM_CLT_response->respond.content.object.books->name)); 
            CU_ASSERT_EQUAL(0, strcpy("This is book1", f_test_CNM_CLT_response->respond.content.object.books->desp)); 
        }
    } 
        
        
    result = eLibrary_CNM_CLT_destroyRequest(request_handle); 
    
    request_data.retrieval_type = RETRIEVAL_TYPE_RM_FRIEND;
    request_data.sort_id = SORT_INVALID;
    request_data.filter_id = FILTER_BY_USER_NAME;
    request_data.object_str = "Nancy";    
    result = eLibrary_CNM_CLT_createRequest(request_data, &request_handle);
    result = eLibrary_CNM_CLT_sendRequest(request_handle); 
    test_wait_sem(f_test_CNM_CLT); 
    CU_ASSERT_EQUAL(request_handle, f_test_CNM_CLT_returned_request); 
    if (NULL != f_test_CNM_CLT_response)
    {
        CU_ASSERT_EQUAL(RETRIEVAL_TYPE_BROWSE_BOOKS, f_test_CNM_CLT_response->retrieval_type);
        CU_ASSERT_EQUAL(SYSTEM_STATUS_OK, f_test_CNM_CLT_response->respond.response_status);
    } 
    result = eLibrary_CNM_CLT_destroyRequest(request_handle); 
    
    eLibrary_CNM_CLT_term();
}

CU_TestInfo test_CNM_client_tests[] = {
  { "server test", test_CNM_SHM_client},
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
