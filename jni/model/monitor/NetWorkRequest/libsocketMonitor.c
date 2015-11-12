/*
 * socket.c
 *
 *  Created on: 2015年11月11日
 *      Author: peng
 */
#include "libsocketMonitor.h"
SocketMonitor socketMonitorObj = {0};
SocketMonitor *socketMonitor = &socketMonitorObj;

//void *replace_func
ssize_t replace_send(int sockfd, const void *buf, size_t len, int flags){

	LOGD(">>> replace_send()");
	return send(sockfd, buf, len, flags);
}

ssize_t replace_recv(int sockfd, void *buf, size_t len, int flags){

	LOGD(">>> replace_recv()");
	return 1;
}

ssize_t replace_sendto(int sockfd, const void *buf, size_t len, int flags,
			  const struct sockaddr *dest_addr, socklen_t addrlen){

	LOGD(">>> replace_sendto()");
	return 1;
}

ssize_t replace_recvfrom(int sockfd, void *buf, size_t len, int flags,
				struct sockaddr *src_addr, socklen_t *addrlen){

	LOGD(">>> replace_recvfrom()");
	return 1;
}

int registerMonitor(){
	_FUNC_START
	do {

	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END_DEFAULT
}

int start(){

	_FUNC_START
	do {

	// 		const char *soname, const char *symbol, void *replace_func, void **old_func

	//		ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	//
	//	    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
	//					  const struct sockaddr *dest_addr, socklen_t addrlen);
	//		ssize_t recv(int sockfd, void *buf, size_t len, int flags);
	//
	//		ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
	//						struct sockaddr *src_addr, socklen_t *addrlen);

		socketMonitor->elfHook->elfHook("libc.so", "send", (void *)replace_send, (void **)send);
	//	socketMonitor->elfHook->elfHook("libc.so", "sendto", (void *)replace_sendto, sendto);
	//	socketMonitor->elfHook->elfHook("libc.so", "recvfrom", (void *)replace_recvfrom, recvfrom);
	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END_DEFAULT
	return 1;
}
