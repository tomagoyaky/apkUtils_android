
#include "jni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hook.h"

int main(int argc, char **argv) {

	if(argc != 5){
		printf("[-]args's length(%d) is not right", argc);
		return -1;
	}
	pid_t target_pid = atoi(argv[1]);
	char *payloadLib = argv[2];
	char *functionName = argv[3];
	char *parameter = argv[4];

	printf("[+]    target_pid | %d\n", target_pid);
	printf("[+]    payloadLib | %s\n", payloadLib);
	printf("[+]  functionName | %s\n", functionName);
	printf("[+]     parameter | %s\n", parameter);

//	pid_t target_pid = find_pid_of("/system/bin/servicemanager");
//	int success = inject_remote_process(target_pid,
//		"/data/local/tmp/hello.so",
//		"hook_entry",
//		"I'm parameter!",
//		strlen("I'm parameter!"));

	int success = inject_remote_process(
		target_pid,
		payloadLib,
		functionName,
		parameter,
		strlen(parameter));
	if(success < 0)
		printf("[-] Not success.");
	return 0;
}
