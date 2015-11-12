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

int ptrace_readdata(pid_t pid, unsigned char *src, unsigned char *buf, unsigned int size);
int ptrace_writedata(pid_t pid, unsigned char *dest, unsigned char *data, unsigned int size);
int ptrace_writestring(pid_t pid, unsigned char *dest, char *str);
int ptrace_call(pid_t pid, unsigned int addr, long *params, unsigned int num_params, struct pt_regs* regs);
int ptrace_getregs(pid_t pid, struct pt_regs* regs);
int ptrace_setregs(pid_t pid, struct pt_regs* regs);
int ptrace_continue(pid_t pid);
int ptrace_attach(pid_t pid);
int ptrace_detach(pid_t pid);
void print_regs(pt_regs *regs);
void* get_module_base(pid_t pid, const char* module_name);
void* get_remote_addr(pid_t target_pid, const char* module_name, void* local_addr);
int inject_remote_process(pid_t target_pid, const char *library_path, const char *function_name, const char *param, size_t param_size);
int find_pid_of(const char *process_name);

#ifdef __cplusplus
}
#endif

struct inject_param_t
{
	pid_t from_pid;
};
