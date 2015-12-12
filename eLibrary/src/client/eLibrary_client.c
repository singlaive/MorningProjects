/*
 * eLibrary_client.c
 *
 *  Created on: 15 Aug 2013
 *      Author: singlaive
 */

int main(void)
{
	eLibrary_CLT_registerClient();
	for (;;)
	{
		eLibrary_CLT_createRequest(str, &request);
		cache_hit = readCache(request, &response);
		if (false == cache_hit)
		{
			eLibrary_CLT_getResponse();
		}
		fillCache(response);
		writeOutput();

		freeResponse();
	}
	eLibrary_CLT_destroyClient();
}

















