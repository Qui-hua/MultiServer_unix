/*
 *	CHEN SEI-LIM
 *	2017-01-24
 */

#ifndef	_CONFIG_H_
#define	_CONFIG_H_

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h> 
#include <stdarg.h>

#define	EXPORT
#define	BACK_LOG	20

#define	FALSE		0
#define	TRUE		1

#define	uint8		unsigned char
#define	uint16		unsigned short
#define	uint32		unsigned int
#define	uint64		unsigned long long

#define	int8		char
#define	int16		short
#define	int32		int
#define	int64		long long


#endif	/* _CONFIG_H_ */
