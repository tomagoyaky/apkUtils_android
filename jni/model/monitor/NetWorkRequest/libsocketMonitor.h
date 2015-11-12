/*
 * libsocketMonitor.h
 *
 *  Created on: 2015年11月11日
 *      Author: peng
 */

#ifndef MODEL_MONITOR_NETWORKREQUEST_LIBSOCKETMONITOR_H_
#define MODEL_MONITOR_NETWORKREQUEST_LIBSOCKETMONITOR_H_

#include "support/Mcro_Common.h"
#include "ElfHook/elfhook.h"

#include <sys/types.h>
#include <sys/socket.h>

typedef int socklen_t;
typedef struct _SocketMonitor{
	ELFHook *elfHook;
	int (*registerMonitor)();
	int (*start)();
}SocketMonitor;

//void *replace_func
ssize_t replace_send(int sockfd, const void *buf, size_t len, int flags);

ssize_t replace_sendto(int sockfd, const void *buf, size_t len, int flags,
			  const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t replace_recv(int sockfd, void *buf, size_t len, int flags);


ssize_t replace_recvfrom(int sockfd, void *buf, size_t len, int flags,
				struct sockaddr *src_addr, socklen_t *addrlen);

int registerMonitor();

int start();

#endif /* MODEL_MONITOR_NETWORKREQUEST_LIBSOCKETMONITOR_H_ */
