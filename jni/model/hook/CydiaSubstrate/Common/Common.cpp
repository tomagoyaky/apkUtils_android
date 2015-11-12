#include "stdio.h"
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include "substrate.h"

/**
 *	获取进程名称
 * @param pid_t 输入进程号
 * @return char* 进程名
 */
char* getpidName(pid_t inpid){
	char proc_pid_path[1024];
	char buf[1024];
	char* outname = NULL;
	memset(buf,0,1024);
	memset(proc_pid_path,0,1024);
	sprintf(proc_pid_path, "/proc/%d/status", inpid);
	FILE* fp = fopen(proc_pid_path, "r");
	if(NULL != fp){
		if( fgets(buf, 1023, fp)== NULL ){
			fclose(fp);
		}
		fclose(fp);
		outname = (char*)malloc(1024);
		memset(outname,0,1024);
		sscanf(buf, "%*s %s", outname);
	}
	return outname;
}
/**
 *	获取配置文件
 * @param pid_t 输入进程号
 * @return char* 进程名
 */
 char* getConfig(){
	FILE* fd =fopen("/sdcard/MHookDemo/Config.txt","r");
	//SD卡读取失败，接着读取data/local这个目录还是要设置
	if(fd == NULL){
		if(fd == NULL){
			return NULL;
		}
	}
	char* mConfig = (char*)malloc(10240);
	memset(mConfig,0,10240);
	fgets(mConfig,10240,fd);
	fclose(fd);
	size_t mConfigSize = strlen(mConfig);
	if(mConfigSize == 0){
		free(mConfig);
		return NULL;
	}
	return mConfig;
}
/**
 *	获取配置文件
 * @return char* 进程名
 */
 void trim(char *s)
{
	int len = strlen(s);
	/* trim trailing whitespace */
	while ( len > 0 && isspace(s[len-1]))
	      s[--len]='\0';
	/* trim leading whitespace */
	memmove(s, &s[strspn(s, " \n\r\t\v")], len);
}
char* getSelfName(){
	FILE* fd =fopen("/proc/self/status","r");
	if(fd == NULL)return NULL;
	char* mBuf = (char*)malloc(1024);	
	memset(mBuf,0,1024);
	fgets(mBuf,1024,fd);
	if((strlen(mBuf) == 0)||(strstr(mBuf,":") == NULL)){
		free(mBuf);
		return NULL;
	}
	char* mOut = strdup(strstr(mBuf,":")+1);
	free(mBuf);
	trim(mOut);
	return mOut;
}
