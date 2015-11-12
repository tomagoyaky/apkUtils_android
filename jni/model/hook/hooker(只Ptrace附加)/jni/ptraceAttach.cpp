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

void print_regs(pt_regs *regs){
	int i = 0;
	for (i; i < 18; i++)
		printf("reg_v%02d=%08x\t", i, regs->uregs[i]);
	printf("\n");
}


int main(int argc, char **argv) {

	if(argc != 2){
		printf("[-]args's length(%d) is not right", argc);
		return -1;
	}
	pid_t target_pid = atoi(argv[1]);
	printf("[+]    target_pid | %d\n", target_pid);

	ptrace_attach(target_pid);

	struct pt_regs regs;
	ptrace_getregs(target_pid, &regs);
	print_regs(&regs);

	ptrace_detach(target_pid);
}
