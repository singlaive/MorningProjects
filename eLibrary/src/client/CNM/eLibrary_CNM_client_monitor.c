
void mock_client_create_request(char *user_name,
	       			char *password, 
				COMMON_HANDLE client_id, 
				RETRIEVAL_TYPE retrieval_type, 
				char *json_string, 
				char *request_buf)
{
	memcpy(request_buf, user_name, MAX_NAME_LENGTH);
	memcpy(request_buf+MAX_NAME_LENGTH, password, MAX_PASSWORD_LENGTH);
	memcpy(request_buf+MAX_NAME_LENGTH+MAX_PASSWORD_LENGTH, client_id, sizeof(client_id));
	memcpy(request_buf+MAX_NAME_LENGTH+MAX_PASSWORD_LENGTH+sizeof(client_id), retrieval_type, sizeof(retrieval_type));
	memcpy(request_buf+MAX_NAME_LENGTH+MAX_PASSWORD_LENGTH+sizeof(client_id)+sizeof(retrieval_type), json_string, strlen(json_string));
}

void mock_client_read_response(char response_str,
				COMMON_HANDLE *p_client_id, 
				RETRIEVAL_TYPE *p_retrieval_type, 
				char *json_string) 
{
	memcpy(p_client_id, response_str, sizeof(COMMON_HANDLE));
	memcpy(p_retrieval_type, response_str+sizeof(COMMON_HANDLE), sizeof(RETRIEVAL_TYPE);
	memcpy(json_string, response_str+sizeof(COMMON_HANDLE)+sizeof(RETRIEVAL_TYPE), MAX_JSON_LEGNTH);
}


mock_client_monitor_thread(void)
{
	while (poll(fd_table, 1, -1) > 0)
	{
		if ((fd_table.revents & (uint16_t)POLLIN) || (fd_table.revents & (uint16_t)POLLPRI))
		{
			read(test_fifo, buf, MAX_BUF);
			memcpy(&client_id, buf, sizeof(client_id));
			memcpy(response_json, buf+sizeof(client_id), MAX_JSON_LENGTH);
			eLibrary_JSON2C(response_json, &response_data);
			CU_ASSERT_NOT_EQUAL(0U, client_id);
			CU_ASSERT_EQUAL(RETRIEVAL_TYPE_CREATE_USER, response_data.retrieval_type);
			CU_ASSERT_EQUAL(COMMON_STATUS_OK, response_data.respond.response_status);
		}
	}
}
