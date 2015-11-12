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
#include "hook.h"

int ptrace_getregs(pid_t pid, struct pt_regs* regs){
	if (ptrace(PTRACE_GETREGS, pid, NULL, regs)< 0){
		printf("[-] ptrace_getregs: Can not get register values: %s\n", strerror(errno));
		exit(Error_Ptrace_GetRegister);
	}
	return 0;
}

int ptrace_setregs(pid_t pid, struct pt_regs* regs){
	if (ptrace(PTRACE_SETREGS, pid, NULL, regs)< 0){
		printf("[-] ptrace_setregs: Can not set register values: %s\n", strerror(errno));
		exit(Error_Ptrace_SetRegister);
	}
	return 0;
}

int ptrace_continue(pid_t pid){
	if (ptrace(PTRACE_CONT, pid, NULL, 0)< 0){
		printf("[-] ptrace_cont: Can not ptrace continue: %s\n", strerror(errno));
		exit(Error_Ptrace_Continue);
	}
	return 0;
}

int ptrace_attach(pid_t pid){
	if (ptrace(PTRACE_ATTACH, pid, NULL, 0)< 0){
		printf("[-] PTRACE_ATTACH failed: %s\n", strerror(errno));
		exit(Error_Ptrace_Attach);
	}
	waitpid(pid, NULL, WUNTRACED);
	printf("[+] PTRACE_ATTACH SUCCESS\n");
	// 继续进程标志为 pid 的被调试进程的执行
	if (ptrace(PTRACE_SYSCALL, pid, NULL, 0)< 0){
		printf("[-] ptrace_syscall failed: %s\n", strerror(errno));
		exit(Error_Ptrace_Call);
	}
	waitpid(pid, NULL, WUNTRACED);
	return 0;
}

int ptrace_detach(pid_t pid){
	if (ptrace(PTRACE_DETACH, pid, NULL, 0)< 0){
		printf("[-] PTRACE_DETACH failed: %s\n", strerror(errno));
		exit(Error_Ptrace_Detach);
	}
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

/**
 * 打印寄存器
 * */
void print_regs(pt_regs *regs){
	for (int i = 0; i < 18; i++)
		printf("reg_v%02d=%08X\t", i, regs->uregs[i]);
	printf("\n");
}
/*---------------------------------------------------------------------------------------------*/
int getCurrentTime(){
	/*
	 * time ( &rawtime ); -- 获取时间，以秒计，从1970年1月一日起算，存于rawtime
	 * localtime ( &rawtime ); -- 转为当地时间，tm 时间结构
	 * asctime （）-- 转为标准ASCII时间格式：
	 * */
	time_t rawtime = clock();;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	return timeinfo->tm_sec;
}
/**
 * 以十六进制打印区块
 * */
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen) {
	char ddl, ddh;
	int i;

	for (i = 0; i < nLen; i++) {
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57)
			ddh = ddh + 7;
		if (ddl > 57)
			ddl = ddl + 7;
		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}
}

void print_hex(const char *dataName, long base, unsigned char *dst, unsigned int size){

	if(size == 0) return;
	printf("+ --------------------------------\n");
	printf("+ %s\n", dataName);
	printf("+ Address:0x%08X, Size:%08X\n", base, size);
	printf("+ ----------------------------------\n");
	int count = -1;
	int flag = 0;
	for (int i = 0; i < size; i++)
	{
		// 打印地址偏移
		if(i % 16 == 0){
			flag = 1;
			count++;
			printf("\n[0x%08X] +%08X ", base + 16 * count, 16 * count);
		}
		if(i % 4 == 0)
		{
			// 打印间隔
			printf("|");
		}
		// 打印数据
		printf("%02X ", dst[i]);

		int cur = i;
		int j = 0;
		if((i + 1) % 16 == 0 && flag == 1)
		{
			printf("|");
			for (j = cur; j < cur + 16; j++)
				printf("%c", dst[j]);
			flag = 0;
		}
	}
	printf("\n");
}

/*
 * 获取pid所代表的heap、stack、vectors空间
 */
void getSpaceAddress(pid_t pid, const char *region, long *startAddress_ptr, long *endAddress_ptr){

	// 00011000-00022000 rw-p 00000000 00:00 0          [heap]
	// be9bc000-be9dd000 rw-p 00000000 00:00 0          [stack]
	// ffff0000-ffff1000 r-xp 00000000 00:00 0          [vectors]
	char* address;
	long addr = 0;
	char path[256];
	int buffer_size = 1024;
	char _region[256];
	sprintf(_region, "[%s]", region);

	char *buffer = (char*)malloc(buffer_size);
	memset(buffer, 0, buffer_size);
	sprintf(path, "/proc/%d/maps", pid);
	FILE *fp = fopen(path, "r");

	memset(buffer, 0, buffer_size);
	if (fp != NULL){
		while (fgets(buffer, buffer_size, fp)){
			if (strstr(buffer, _region)){ // 判断名称region是否在这一行
				printf("[+] %s\n", buffer);
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

void SearchRegion(long base, int coo, unsigned char *bufItem, size_t size, const char *key, size_t len){

	time_t startTime = clock();
	int totalNum = 0;
	printf("[+] [Thread:%02d] Search Region [0x%08X]-[0x%08X] with key '%s', ken's length=%d.\n", coo, base, base + size, key, len);
	for (int i = 0; i < size; ++i) {
		for(int j = 0; j < len; j++){

			// 若首字母相同, 从buf中获取同key一样大小的字符串出来
			if(bufItem[i] == key[j]){
				char tempBuffer[len + 1];
				for(int k = 0; k < len; k++){
					tempBuffer[k] = bufItem[i + k];
				}
				tempBuffer[len] = '\0'; // 由于是比较字符串,添加\0结尾符
				if(strcmp(tempBuffer, key) == 0){
					totalNum++;
//					printf("[-] [%s] --- [%s]\n", tempBuffer, key);
					printf("[+] >>> OK <<< find same data with '%s', at address 0x%08X.\n", key, base + i);
				}else{
//					printf("[-] [%s] --- [%s]\n", tempBuffer, key);
				}
			}
		}
	}
	if(totalNum == 0)
		printf("[+] >>> Failure <<< \n[+] Can not find key '%s' in Region [0x%08X]-[0x%08X].\n", key, base, base + size);
	time_t endTime = clock();
	printf("[+] total found apprence %d times, take %.5f(s)\n", totalNum, difftime(endTime,startTime) / CLOCKS_PER_SEC);
}
/*
 * base 用于打印的时候显示内存地址，该值为一个long的内存地址
 * buffer 即将要搜索的数据
 * size 数据的大小
 * key 关键字符串
 * regionNum 按区块搜索的数目
 * */
void SearchDataWithKey(long base, unsigned char *buffer, size_t size, char *key, int regionNum){

	if(size == 0) return;
	// 判断是否需要划分块
	if(regionNum != 1){
		if(size > 0  						&& size <= 1024 * 1024					) regionNum = 3;
		if(size > 1024 * 1024 				&& size <= 1024 * 1024 * 512			) regionNum = 6;
		if(size > 1024 * 1024 * 512 		&& size <= 1024 * 1024 * 1024			) regionNum = 9;
		if(size > 1024 * 1024 * 1024 		&& size <= 1024 * 1024 * 1024 * 100		) regionNum = 12;
		if(size > 1024 * 1024 * 1024 * 100 	&& size <= 1024 * 1024 * 1024 * 512		) regionNum = 15;
		if(size > 1024 * 1024 * 1024 * 512 	&& size <= 1024 * 1024 * 1024 * 1024	) regionNum = 18;
		else regionNum = 20; // 最多分10个区
		// 划分块
		int regionSize = size / regionNum;
		int lastRegionSize = size % regionNum;

		printf("[+] Region size=0x%08X, use %d thread to do. regionSize=%d, lastRegionSize=%d\n", size, regionNum, regionSize , lastRegionSize);
		for(int i = 0; i < regionNum; i++){
			pid_t pid = fork();
			int inc = 0;
			if(pid < 0) {return;}
			if(pid == 0){
				unsigned char *bufItem = NULL;
				if(i == (regionNum - 1))
					inc = i * lastRegionSize;
				else
					inc = i * regionSize;
				bufItem = buffer + inc;
				SearchRegion(base, i + 1, bufItem, inc, key, strlen(key));
			}
		}
	}else{
		printf("[+] Region size=0x%08X, use %d thread to do.\n", size, regionNum);
		unsigned char *bufItem = NULL;
		SearchRegion(base, 1, buffer, size, key, strlen(key));
	}
}

void SearchDataWithKey(long base, unsigned char *buffer, size_t size, char *key){
	SearchDataWithKey(base, buffer, size, key, 1);
}
void SearchSegment(pid_t target_pid, const char *info, char* key, long address_start, long address_end){

	printf("[+]------------------------------------------------------------------------------------------------------\n");
	size_t size = address_end - address_start;
	printf("[+] [%s] [address_end]0x%08x - [address_start]0x%08x = [size]0x%08x\n", info, address_end, address_start, size);
	if((int)size < 0){
		printf("[-] 'address_end:0x%08X' must be more than 'address_start:0x%08X'\n", address_end, address_start);
		return;
	}
	unsigned char *buffer = (unsigned char *)malloc(size);
	memset(buffer, 0, size);
	ptrace_readdata(target_pid, (unsigned char *)address_start, buffer, size);
	SearchDataWithKey(address_start, buffer, size, key, 				1);
//	print_hex(info, address_start, buffer, size);
	free(buffer);
}

// 获取堆栈、变量区的区域地址, 如果放到函数里面请注意初始化下
long heap_start, heap_end;
long statck_start, statck_end;
long vectors_start, vectors_end;

int main(int argc, char **argv) {
#define Debug 1
	if(argc != 3){
		printf("[-]args's length(%d) is not right\n", argc);
		return -1;
	}
	pid_t target_pid 	= atoi(argv[1]);
	int targetInteger 	= atoi(argv[2]);
	printf("[+]    target_pid | %d\n", target_pid);

	ptrace_attach(target_pid);
	struct pt_regs regs;
	ptrace_getregs(target_pid, &regs);

	printf("[+] print regs:\n");
#if Debug
	print_regs(&regs);
#endif
	printf("[+] search target integer:%d\n", targetInteger);
	getSpaceAddress(target_pid, "heap", 	&heap_start, &heap_end);
	getSpaceAddress(target_pid, "stack", 	&statck_start, &statck_end);
	getSpaceAddress(target_pid, "vectors", 	&vectors_start, &vectors_end);
	// 打印区域地址
	printf("[+]heap:0x%08X - 0x%08X\n", heap_start, heap_end);
	printf("[+]statck:0x%08X - 0x%08X\n", statck_start, statck_end);
	printf("[+]vectors:0x%08X - 0x%08X\n", vectors_start, vectors_end);
	// 搜索段
	SearchSegment(target_pid, "heap region data", "PK", heap_start, heap_end);
	SearchSegment(target_pid, "statck region data", "PK", statck_start, statck_end);
	SearchSegment(target_pid, "vectors region data", "PK", vectors_start, vectors_end);

	SearchSegment(target_pid, "heap region data", "dex", heap_start, heap_end);
	SearchSegment(target_pid, "statck region data", "dex", statck_start, statck_end);
	SearchSegment(target_pid, "vectors region data", "dex", vectors_start, vectors_end);

	SearchSegment(target_pid, "heap region data", "com.", heap_start, heap_end);
	SearchSegment(target_pid, "statck region data", "com.", statck_start, statck_end);
	SearchSegment(target_pid, "vectors region data", "com.", vectors_start, vectors_end);

	SearchSegment(target_pid, "heap region data", "android", heap_start, heap_end);
	SearchSegment(target_pid, "statck region data", "android", statck_start, statck_end);
	SearchSegment(target_pid, "vectors region data", "android", vectors_start, vectors_end);
}
