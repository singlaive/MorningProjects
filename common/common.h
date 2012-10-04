/*
 * common.h
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

/* Modification history:
 *     11th Apr 2012 Tree implementation
 *     6th May 2012 Hash table implementation
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define STATUS_OK		0x00000000
#define STATUS_FAIL		0x00000001
#define STATUS_INVALID_PARAM 0x00000002
#define STATUS_DATABASE_FULL 0x00000003
#define STATUS_DATABASE_NOT_FOUND 0x00000004
#define STATUS_DATABASE_DUPLICATED 0x00000005
#define STATUS_DATABASE_OVERWRITEN 0x00000006

#define HANDLE_BASE_TREE	    0x10000000 /* Base value for generating a handle for tree appliance. */
#define HANDLE_BASE_HASH_TABLE	0x01000000 /* Base value for generating a handle for hash table appliance. */
#define HANDLE_COUNTER_MASK 	0x000000FF /* Mask to get the index to the data array. */

#define MAX_OBJECTS			128U /* Maximal num of objects for each type of data. For example, there are such no of trees that can be created. */
#define MAX_CMD_LENGTH		12U

#define DIAG_INFO(msg, arg...)	printf("INFO %s() %d: " msg "\n", __FILE__, __LINE__, ##arg);
#define DIAG_PARAM(msg, arg...)	printf("PARAM %s() %d: " msg "\n", __FILE__, __LINE__, ##arg);
#define DIAG_ERROR(msg, arg...)	printf("ERROR %s() %d: " msg "\n", __FILE__, __LINE__, ##arg);
#define DIAG_WARN(msg, arg...)	printf("WARN %s() %d: " msg "\n", __FILE__, __LINE__, ##arg);
#define DIAG_FATAL(func_called, arg...)	printf("FATAL %s() failed (file %s; line %d; errno %d): " func_called "\n", __FUNCTION__, __LINE__, errno, ##arg);

/********************************************************************/
/* 						Structs, Unions, etc.						*/
/********************************************************************/

typedef uint32_t SYSTEM_STATUS;

void *p_trees[MAX_OBJECTS];
void *p_hashtables[MAX_OBJECTS];

typedef struct cmd
{
	char cmd[MAX_CMD_LENGTH];
	uint32_t num_options;
	char *option; 			/* Array of options. */
	uint32_t num_params;
	char **params;			/* Array of params. No limitation about length of each parameter. */
}cmd_line_tag;

/********************************************************************/
/* 							Func prototypes							*/
/********************************************************************/

void fgets_convert(char * str);

/* Make up a integer handle for any given element.
 * ptr 		[in] pointer referencing a user defined data
 * p_handle [out] handle indicating the data.
 */
int COMMON_handle_create(void *ptr, uint8_t *p_handle);

/* Retrieve the address to the user data indicated by a given handle.
 * handle [in] handle indicating a data
 * ptr	  [out] the user data
 **/
int COMMON_object_retrieve(uint8_t handle, void **ptr);

/* Format the commend line input
 * .
 * str [in] string to be parsed. It must not be stored in static address.
 * p_commend_line [out] parsed commends.
 **/
SYSTEM_STATUS parse_cmd(char *str, cmd_line_tag *p_commend_line);

#if 0
00036 #define EGENERIC      (_SIGN 99)  /* generic error */
00037 #define EPERM         (_SIGN  1)  /* operation not permitted */
00038 #define ENOENT        (_SIGN  2)  /* no such file or directory */
00039 #define ESRCH         (_SIGN  3)  /* no such process */
00040 #define EINTR         (_SIGN  4)  /* interrupted function call */
00041 #define EIO           (_SIGN  5)  /* input/output error */
00042 #define ENXIO         (_SIGN  6)  /* no such device or address */
00043 #define E2BIG         (_SIGN  7)  /* arg list too long */
00044 #define ENOEXEC       (_SIGN  8)  /* exec format error */
00045 #define EBADF         (_SIGN  9)  /* bad file descriptor */
00046 #define ECHILD        (_SIGN 10)  /* no child process */
00047 #define EAGAIN        (_SIGN 11)  /* resource temporarily unavailable */
00048 #define ENOMEM        (_SIGN 12)  /* not enough space */
00049 #define EACCES        (_SIGN 13)  /* permission denied */
00050 #define EFAULT        (_SIGN 14)  /* bad address */
00051 #define ENOTBLK       (_SIGN 15)  /* Extension: not a block special file */
00052 #define EBUSY         (_SIGN 16)  /* resource busy */
00053 #define EEXIST        (_SIGN 17)  /* file exists */
00054 #define EXDEV         (_SIGN 18)  /* improper link */
00055 #define ENODEV        (_SIGN 19)  /* no such device */
00056 #define ENOTDIR       (_SIGN 20)  /* not a directory */
00057 #define EISDIR        (_SIGN 21)  /* is a directory */
00058 #define EINVAL        (_SIGN 22)  /* invalid argument */
00059 #define ENFILE        (_SIGN 23)  /* too many open files in system */
00060 #define EMFILE        (_SIGN 24)  /* too many open files */
00061 #define ENOTTY        (_SIGN 25)  /* inappropriate I/O control operation */
00062 #define ETXTBSY       (_SIGN 26)  /* no longer used */
00063 #define EFBIG         (_SIGN 27)  /* file too large */
00064 #define ENOSPC        (_SIGN 28)  /* no space left on device */
00065 #define ESPIPE        (_SIGN 29)  /* invalid seek */
00066 #define EROFS         (_SIGN 30)  /* read-only file system */
00067 #define EMLINK        (_SIGN 31)  /* too many links */
00068 #define EPIPE         (_SIGN 32)  /* broken pipe */
00069 #define EDOM          (_SIGN 33)  /* domain error       (from ANSI C std) */
00070 #define ERANGE        (_SIGN 34)  /* result too large   (from ANSI C std) */
00071 #define EDEADLK       (_SIGN 35)  /* resource deadlock avoided */
00072 #define ENAMETOOLONG  (_SIGN 36)  /* file name too long */
00073 #define ENOLCK        (_SIGN 37)  /* no locks available */
00074 #define ENOSYS        (_SIGN 38)  /* function not implemented */
00075 #define ENOTEMPTY     (_SIGN 39)  /* directory not empty */
00076 #define ELOOP         (_SIGN 40)  /* too many levels of symlinks detected */
00077
00078 /* The following errors relate to networking. */
00079 #define EPACKSIZE     (_SIGN 50)  /* invalid packet size for some protocol */
00080 #define EOUTOFBUFS    (_SIGN 51)  /* not enough buffers left */
00081 #define EBADIOCTL     (_SIGN 52)  /* illegal ioctl for device */
00082 #define EBADMODE      (_SIGN 53)  /* badmode in ioctl */
00083 #define EWOULDBLOCK   (_SIGN 54)
00084 #define EBADDEST      (_SIGN 55)  /* not a valid destination address */
00085 #define EDSTNOTRCH    (_SIGN 56)  /* destination not reachable */
00086 #define EISCONN       (_SIGN 57)  /* all ready connected */
00087 #define EADDRINUSE    (_SIGN 58)  /* address in use */
00088 #define ECONNREFUSED  (_SIGN 59)  /* connection refused */
00089 #define ECONNRESET    (_SIGN 60)  /* connection reset */
00090 #define ETIMEDOUT     (_SIGN 61)  /* connection timed out */
00091 #define EURG          (_SIGN 62)  /* urgent data present */
00092 #define ENOURG        (_SIGN 63)  /* no urgent data present */
00093 #define ENOTCONN      (_SIGN 64)  /* no connection (yet or anymore) */
00094 #define ESHUTDOWN     (_SIGN 65)  /* a write call to a shutdown connection */
00095 #define ENOCONN       (_SIGN 66)  /* no such connection */
00096 #define EAFNOSUPPORT  (_SIGN 67)  /* address family not supported */
00097 #define EPROTONOSUPPORT (_SIGN 68) /* protocol not supported by AF */
00098 #define EPROTOTYPE    (_SIGN 69)  /* Protocol wrong type for socket */
00099 #define EINPROGRESS   (_SIGN 70)  /* Operation now in progress */
00100 #define EADDRNOTAVAIL (_SIGN 71)  /* Can't assign requested address */
00101 #define EALREADY      (_SIGN 72)  /* Connection already in progress */
00102 #define EMSGSIZE      (_SIGN 73)  /* Message too long */
00103 #define ENOTSOCK      (_SIGN 74)  /* Socket operation on non-socket */
00104 #define ENOPROTOOPT   (_SIGN 75)  /* Protocol not available */
00105
00106 /* The following are not POSIX errors, but they can still happen.
00107  * All of these are generated by the kernel and relate to message passing.
00108  */
00109 #define ELOCKED      (_SIGN 101)  /* can't send message due to deadlock */
00110 #define EBADCALL     (_SIGN 102)  /* illegal system call number */
00111 #define EBADSRCDST   (_SIGN 103)  /* bad source or destination process */
00112 #define ECALLDENIED  (_SIGN 104)  /* no permission for system call */
00113 #define EDEADSRCDST  (_SIGN 105)  /* source or destination is not alive */
00114 #define ENOTREADY    (_SIGN 106)  /* source or destination is not ready */
00115 #define EBADREQUEST  (_SIGN 107)  /* destination cannot handle request */
00116 #define ESRCDIED     (_SIGN 108)  /* source just died */
00117 #define EDSTDIED     (_SIGN 109)  /* destination just died */
00118 #define ETRAPDENIED  (_SIGN 110)  /* IPC trap not allowed */
00119 #define EDONTREPLY   (_SIGN 201)  /* pseudo-code: don't send a reply */
#endif
#endif /* COMMON_H_ */
