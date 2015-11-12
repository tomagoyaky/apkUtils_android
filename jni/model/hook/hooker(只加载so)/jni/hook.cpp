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
#include <asm/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "hook.h"

#define CPSR_T_MASK (1u << 5)
#define MAX_PATH 0x100
#define REMOTE_ADDR(addr, local_base, remote_base)((unsigned int)(addr)+ (unsigned int)(remote_base)- (unsigned int)(local_base))

const char *libc_path = "/system/lib/libc.so";
const char *linker_path = "/system/bin/linker";

int ptrace_readdata(pid_t pid, unsigned char* src, unsigned char* buf, unsigned int size){

	unsigned int i, j, remain;
	unsigned char* laddr;
	union u {
		long val;
		char chars[sizeof(long)];
	} d;

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

int ptrace_writedata(pid_t pid, unsigned char* dest, unsigned char* data, unsigned int size){

	unsigned int i, j, remain;
	unsigned char* laddr;
	union u {
		long val;
		char chars[sizeof(long)];
	} d;

	j = size / 4;
	remain = size % 4;

	laddr = data;

	for (i = 0; i < j; i++){
		memcpy(d.chars, laddr, 4);
		ptrace(PTRACE_POKETEXT, pid, dest, (void*)d.val);

		dest += 4;
		laddr += 4;
	}

	if (remain > 0){
		d.val = ptrace(PTRACE_PEEKTEXT, pid, dest, 0);
		for (i = 0; i < remain; i++){
			d.chars[i] = *laddr++;
		}
		ptrace(PTRACE_POKETEXT, pid, dest, (void*)d.val);
	}
	return 0;
}

int ptrace_writestring(pid_t pid, unsigned char *dest, char *str){
	return ptrace_writedata(pid, dest, (unsigned char *)str, strlen(str)+ 1);
}

int ptrace_call(pid_t pid, unsigned int addr, long *params, unsigned int num_params, struct pt_regs* regs){

	unsigned int i;
	for (i = 0; i < num_params && i < 4; i++){
		regs->uregs[i] = params[i];
	}
	//
	// push remained params onto stack
	//
	if (i < num_params){
		regs->ARM_sp-= (num_params - i)* sizeof(long);
		ptrace_writedata(pid, (unsigned char *)regs->ARM_sp, (unsigned char*)&params[i], (num_params - i)* sizeof(long));
	}

	regs->ARM_pc= addr;
	if (regs->ARM_pc& 1)
	{
		/* thumb */
		regs->ARM_pc &= (~1u);
		regs->ARM_cpsr |= CPSR_T_MASK;
	}
	else
	{
		/* arm */
		regs->ARM_cpsr &= ~CPSR_T_MASK;
	}

	regs->ARM_lr= 0;

	if (ptrace_setregs(pid, regs)== -1 || ptrace_continue(pid)== -1){
		printf("[-] failed to call function: %s", strerror(errno));
		exit(Error_Ptrace_Call);
	}
	waitpid(pid, NULL, WUNTRACED);
	return 0;
}

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

/**
 * 打印寄存器
 * */
void print_regs(pt_regs *regs){
	int i = 0;
	for (i; i < 18; i++)
		printf("reg_v%02d=%08x\n", i, regs->uregs[i]);
}

void* get_module_base(pid_t pid, const char* module_name){
	FILE *fp;
	long addr = 0;
	char *pch;
	char filename[32];
	char line[1024];

	if (pid < 0){
		/* self process */
		snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
	} else {
		snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	}
	fp = fopen(filename, "r");
	if (fp != NULL){
		while (fgets(line, sizeof(line), fp)){
			if (strstr(line, module_name)){
				pch = strtok(line, "-");
				addr = strtoul(pch, NULL, 16);

				if (addr == 0x8000)
					addr = 0;
				break;
			}
		}
		fclose(fp);
	}
	return (void*)addr;
}

void* get_remote_addr(pid_t target_pid, const char* module_name, void* local_addr){
	void* local_handle, *remote_handle;
	local_handle = get_module_base(-1, module_name);
	remote_handle = get_module_base(target_pid, module_name);
	return (void*)((unsigned int)local_addr + (unsigned int)remote_handle - (unsigned int)local_handle);
}

long ptrace_retval(struct pt_regs * regs)
{
	#if defined(__arm__)
		return regs->ARM_r0;
	#elif defined(__i386__)
		return regs->eax;
	#else
	#error "Not supported"
	#endif
}

long ptrace_ip(struct pt_regs * regs)
{
	#if defined(__arm__)
		return regs->ARM_pc;
	#elif defined(__i386__)
		return regs->eip;
	#else
	#error "Not supported"
	#endif
}
int ptrace_call_wrapper(pid_t target_pid, const char * func_name, void * func_addr, long * parameters, int param_num, struct pt_regs * regs)
{
	printf("[+] Calling %s in target process.\n", func_name);
    if (ptrace_call(target_pid, (uint32_t)func_addr, parameters, param_num, regs) == -1)
        return -1;

    if (ptrace_getregs(target_pid, regs) == -1)
        return -1;
    printf("[+] Target process returned from %s, return value=%x, pc=%x \n",
            func_name, ptrace_retval(regs), ptrace_ip(regs));
    return 0;
}

int inject_remote_process(pid_t target_pid, const char *library_path, const char *function_name, const char *param, size_t param_size){

	/*
	[1]通过远程进程pid，ATTACH到远程进程。

	[2]获取远程进程寄存器值，并保存，以便注入完成后恢复进程原有状态。

	[3]获取远程进程系统调用mmap、dlopen、dlsym调用地址。

	[4]调用远程进程mmap分配一段存储空间，并在空间中写入so库路径以及函数调用参数。

	[5]执行远程进程dlopen,加载so库。

	[6]执行远程进程dlsym，获取so库中需要执行的函数地址。

	[7]执行远程进程中的函数。

	[7]恢复远程进程寄存器。

	[8]DETACH远程进程。
	 * */
	int ret = -1;
	void*mmap_addr, *dlopen_addr, *dlsym_addr, *dlclose_addr;
	void*local_handle, *remote_handle, *dlhandle;
	unsigned char* map_base;
	unsigned char* dlopen_param1_ptr, *dlsym_param2_ptr, *saved_r0_pc_ptr,
			*inject_param_ptr, *remote_code_ptr, *local_code_ptr;

	struct pt_regs regs, original_regs;
	extern unsigned int _dlopen_addr_s, _dlopen_param1_s, _dlopen_param2_s,
			_dlsym_addr_s, _dlsym_param2_s, _dlclose_addr_s, _inject_start_s,
			_inject_end_s, _inject_function_param_s, _saved_cpsr_s,
			_saved_r0_pc_s;

	unsigned int code_length;
	long parameters[10];

	printf("[+] Injecting process: %d\n", target_pid );
	if (ptrace_attach(target_pid)== -1)
		return EXIT_SUCCESS;

	if (ptrace_getregs(target_pid, &regs)== -1)
		goto exit;
	/* save original registers */
	memcpy(&original_regs, &regs, sizeof(regs));

	mmap_addr = get_remote_addr(target_pid, "/system/lib/libc.so", (void*)mmap);
	printf("[+] Remote mmap address: %x\n", mmap_addr );

	/* call mmap */
	parameters[0] = 0;	// addr
	parameters[1] = 0x4000; // size
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC; // prot
	parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE; // flags
	parameters[4] = 0; //fd
	parameters[5] = 0; //offset
	printf("[+] Calling mmap in target process.\n" );
	if (ptrace_call(target_pid, (unsigned int)mmap_addr, parameters, 6, &regs)
			== -1)
		goto exit;

	if (ptrace_getregs(target_pid, &regs)== -1)
		goto exit;

	printf("[+] Target process returned from mmap, return value=%x, pc=%x \n", regs.ARM_r0, regs.ARM_pc );
	map_base = (unsigned char*)regs.ARM_r0;

	dlopen_addr = get_remote_addr(target_pid, linker_path, (void*)dlopen);
	dlsym_addr = get_remote_addr(target_pid, linker_path, (void*)dlsym);
	dlclose_addr = get_remote_addr(target_pid, linker_path, (void*)dlclose);

	printf("[+] Get imports: dlopen: %x, dlsym: %x, dlclose: %x\n", dlopen_addr, dlsym_addr, dlclose_addr );
	remote_code_ptr = map_base + 0x3C00;
	local_code_ptr = (unsigned char*)&_inject_start_s;

	_dlopen_addr_s = (unsigned int)dlopen_addr;
	_dlsym_addr_s = (unsigned int)dlsym_addr;
	_dlclose_addr_s = (unsigned int)dlclose_addr;
	printf("[+] Inject code start: %x, end: %x\n", local_code_ptr, &_inject_end_s );

	code_length = (unsigned int)&_inject_end_s - (unsigned int)&_inject_start_s;
	dlopen_param1_ptr = local_code_ptr + code_length + 0x20;
	dlsym_param2_ptr = dlopen_param1_ptr + MAX_PATH;
	saved_r0_pc_ptr = dlsym_param2_ptr + MAX_PATH;
	inject_param_ptr = saved_r0_pc_ptr + MAX_PATH;

	/* dlopen parameter 1: library name */
	strcpy((char*)dlopen_param1_ptr, library_path);
	_dlopen_param1_s = REMOTE_ADDR(dlopen_param1_ptr, local_code_ptr, remote_code_ptr);
	printf("[+] _dlopen_param1_s: %x\n", _dlopen_param1_s );

	/* dlsym parameter 2: function name */
	strcpy((char*)dlsym_param2_ptr, function_name);
	_dlsym_param2_s = REMOTE_ADDR(dlsym_param2_ptr, local_code_ptr, remote_code_ptr);
	printf("[+] _dlsym_param2_s: %x\n", _dlsym_param2_s );

	/* saved cpsr */
	_saved_cpsr_s = original_regs.ARM_cpsr;

	/* saved r0-pc */
	memcpy(saved_r0_pc_ptr, &(original_regs.ARM_r0), 16 * 4); // r0 ~ r15
	_saved_r0_pc_s = REMOTE_ADDR(saved_r0_pc_ptr, local_code_ptr, remote_code_ptr);
	printf("[+] _saved_r0_pc_s: %x\n", _saved_r0_pc_s );

	/* Inject function parameter */
	memcpy(inject_param_ptr, param, param_size);
	_inject_function_param_s = REMOTE_ADDR(inject_param_ptr, local_code_ptr, remote_code_ptr);
	printf("[+] _inject_function_param_s: %x\n", _inject_function_param_s );

	printf("[+] Remote shellcode address: %x\n", remote_code_ptr );
	ptrace_writedata(target_pid, remote_code_ptr, local_code_ptr, 0x400);

	memcpy(&regs, &original_regs, sizeof(regs));
	regs.ARM_sp= (long)remote_code_ptr;
	regs.ARM_pc= (long)remote_code_ptr;

	printf("[+] recovery regs.\n");
	ptrace_setregs(target_pid, &regs);

	printf("[+] detach.\n");
	ptrace_detach(target_pid);

	// inject succeeded
	printf("[+] inject succeeded\n");
	ret = 0;

	exit: return ret;
}
