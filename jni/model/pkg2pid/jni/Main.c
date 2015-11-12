/*
 * Main.c
 *
 *  Created on: 2015年11月11日
 *      Author: peng
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void execute(char *cmdLine, char *result){
	FILE * fp = NULL;
	fp=popen(cmdLine, "r");
	char buffer[256] = {0};
	fgets(buffer,sizeof(buffer), fp);
	printf("%s\n",buffer);
	pclose(fp);
	strcpy(result, buffer);
}

int main(int argc, char **argv) {

	if(argc != 2){
		printf("[-]args's length(%d) is not right", argc);
		return -1;
	}
	char *packageName = argv[1];
	printf("packageName:%s\n", packageName);
	char cmdLine[256] = {0};
	char result[256] = {0};
	snprintf(cmdLine, 256, "ps | grep %s", packageName);
	execute(cmdLine, result);

	char tempBuffer[256] = {0};
	strcpy(tempBuffer, result);
	printf("tempBuffer:%s\n", tempBuffer);

	char token = strtok(tempBuffer, "");
//	if(token == NULL){
//		printf("token is NULL\n");
//		return -1;
//	}else{
//		printf("token not NULL\n");
//	}
//
//	printf("token:%s\n", token);
	while (token != NULL ) {
		printf("token:%s\n", token);
		token = strtok(NULL, "");
	}
}
