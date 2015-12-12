/*
 * eLibrary_client_api.h
 *
 *  Created on: 29 Aug 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_CLIENT_API_H_
#define ELIBRARY_CLIENT_API_H_


SYSTEM_STATUS eLibrary_CLT_registerClient();
SYSTEM_STATUS eLibrary_CLT_destroyClient();

/* Parse the command input from the user as a request.
 *
 * [in] - buf
 * [out] - p_request_data
 *
 * Return value: COMMON_STATUS_OK if it works fine
 * 				 COMMON_STATUS_INVALID_PARAM if it failed.
 */
SYSTEM_STATUS eLibrary_CLT_createRequest(const char *buf, CLIENT_REQUEST *p_request_data);

SYSTEM_STATUS eLibrary_CLT_getResponse(const CLIENT_REQUEST *p_request_data, CLIENT_RESPONSE *p_response_data);

bool eLibrary_CLT_readCache(const CLIENT_REQUEST *p_request_data, CLIENT_RESPONSE *p_response_data)

SYSTEM_STATUS eLibrary_fillCache(const CLIENT_RESPONSE *p_response_data);

SYSTEM_STATUS eLibrary_writeOutput();

#endif /* ELIBRARY_CLIENT_API_H_ */
