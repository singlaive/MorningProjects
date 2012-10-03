/*
 * common.c
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

void fgets_convert(char * str, char * str1)
{
	char * ptr = str;

	while (('\n' != *ptr) && ('\n' != *ptr))
	{
		ptr++;
	}

	if ('\n' == *ptr)
	{
		*ptr = 0;
	}

	strcpy(str1, str);
}
