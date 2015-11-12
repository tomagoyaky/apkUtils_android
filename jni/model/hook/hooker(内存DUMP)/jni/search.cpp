/*
 ============================================================================
 Name  : libinject.c
 Author  :
 Version  :
 Copyright :
 Description : Android shared library inject helper
 ============================================================================
 */

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/ptrace.h>
#include <string.h>
#include <asm/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int ptrace_attach(pid_t pid){
	if (ptrace(PTRACE_ATTACH, pid, NULL, 0)< 0){
		printf("[-] PTRACE_ATTACH failed: %s\n", strerror(errno));
		exit(-1);
	}
	waitpid(pid, NULL, WUNTRACED);
	printf("[+] PTRACE_ATTACH SUCCESS\n");
	// 继续进程标志为 pid 的被调试进程的执行
	if (ptrace(PTRACE_SYSCALL, pid, NULL, 0)< 0){
		printf("[-] ptrace_syscall failed: %s\n", strerror(errno));
		exit(-1);
	}
	waitpid(pid, NULL, WUNTRACED);
	return 0;
}
int ptrace_readdata(pid_t pid, unsigned char* src, unsigned char* buf, unsigned int size){

	unsigned int i, j, remain;
	unsigned char* laddr;
	union u {
		long val;
		char chars[sizeof(long)];
	} d;

	if(size == 0) return 0;
	j = size / 4;
	remain = size % 4;
	laddr = buf;

	for (i = 0; i < j; i++){
		d.val = ptrace(PTRACE_PEEKTEXT, pid, src, 0);
		memcpy(laddr, d.chars, 4);
		src += 4;
		laddr += 4;
	}

	if (remain > 0){
		d.val = ptrace(PTRACE_PEEKTEXT, pid, src, 0);
		memcpy(laddr, d.chars, remain);
	}
	return 0;
}

/*
 * 获取pid所代表的模块地址
 */
void getSpaceAddress(pid_t pid, const char *region, long *startAddress_ptr, long *endAddress_ptr){

	char* address;
	long addr = 0;
	char path[256];
	int buffer_size = 1024;

	char *buffer = (char*)malloc(buffer_size);
	memset(buffer, 0, buffer_size);
	sprintf(path, "/proc/%d/maps", pid);
	FILE *fp = fopen(path, "r");

	memset(buffer, 0, buffer_size);
	if (fp != NULL){
		while (fgets(buffer, buffer_size, fp)){
			if (strstr(buffer, region)){ // 判断名称region是否在这一行
				address = strtok(buffer, "-");
				addr = strtoul(address, NULL, 16);
				*startAddress_ptr = addr;

				address = strtok(NULL, " ");
				addr = strtoul(address, NULL, 16);
				*endAddress_ptr = addr;
				break;
			}
		}
		fclose(fp);
	}
}

// 获取堆栈、变量区的区域地址, 如果放到函数里面请注意初始化下
long address_start, address_end;

int main(int argc, char **argv) {
	if(argc != 3){
		printf("[-]args's length(%d) is not right\n", argc);
		return -1;
	}
	pid_t target_pid 	= atoi(argv[1]);
	char *moduleName 	= argv[2];
	printf("[+]    target_pid | %d , %s\n", target_pid, moduleName);

	ptrace_attach(target_pid);
	getSpaceAddress(target_pid, moduleName, 	&address_start, &address_end);

	long size = address_end - address_start;
	printf("[+]address:0x%08X - 0x%08X, size:%ld\n", address_start, address_end, size);
	unsigned char *buffer = (unsigned char *)malloc(size);
	memset(buffer, 0, size);
	printf("[+] read ...\n");
	ptrace_readdata(target_pid, (unsigned char *)address_start, (unsigned char*)buffer, size);

	printf("[+] read finsih.\n");
	char path[256];
//	sprintf(path, "/sdcard/%ld_%ld", address_start, size);
	sprintf(path, "/sdcard/dump.out");
	printf("[+] path='%s'\n", path);
	FILE *fp = fopen(path, "w");

	if(fp != NULL){
		size_t wsize = fwrite(buffer, size, 1, fp);
		if(wsize != size){
			printf("[-] %s\n", strerror(errno));
			return -1;
		}
		printf("[+] Success '%s'\n", path);
	}else{
		printf("[-] %s\n", strerror(errno));
	}
}
