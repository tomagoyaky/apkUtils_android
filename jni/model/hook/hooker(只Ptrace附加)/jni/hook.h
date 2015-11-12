#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define OK 0
#define Error_Need_PidParams -1
#define Error_Ptrace_Attach -2
#define Error_Ptrace_Detach -3
#define Error_Ptrace_GetRegister -4
#define Error_Ptrace_SetRegister -5
#define Error_Ptrace_Continue -6
#define Error_Ptrace_WriteData -7
#define Error_Ptrace_Call -8

int ptrace_getregs(pid_t pid, struct pt_regs* regs);
int ptrace_setregs(pid_t pid, struct pt_regs* regs);
int ptrace_continue(pid_t pid);
int ptrace_attach(pid_t pid);
int ptrace_detach(pid_t pid);
void print_regs(pt_regs *regs);

#ifdef __cplusplus
}
#endif

struct inject_param_t
{
	pid_t from_pid;
};
