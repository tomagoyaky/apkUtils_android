/*
 * Mcro_Common.h
 *
 *  Created on: 2015-10-13
 *      Author: tomagoyaky
 */
#ifndef _MCRO_COMMON_H_
#define _MCRO_COMMON_H_


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <android/log.h>
// 日誌打印控制宏
#define DEBUG 1 //關閉則置爲0
#define ISPRINTF 0 //android日誌打印則置爲0

#define  ANDROID_LOG_VERBOSE 2
#define  ANDROID_LOG_DEBUG 3
#define  ANDROID_LOG_INFO 4
#define  ANDROID_LOG_WARN 5
#define  ANDROID_LOG_ERROR 6

#if DEBUG
	#define LOG_TAG "tomagoyaky"
	#if ISPRINTF
		#define __Debug__ if(0) printf("|--> %s():%d, file={%s}", __FUNCTION__, __LINE__, __FILE__);
		#define LOGD(...) printf(__VA_ARGS__);
		#define LOGI(...) printf(__VA_ARGS__);
		#define LOGV(...) printf(__VA_ARGS__);
		#define LOGW(...) printf(__VA_ARGS__);
		#define LOGE(...) printf(__VA_ARGS__);
		#define LOGF(...) printf(__VA_ARGS__);
	#else
		#define __Debug__ if(1) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "|--> %s():%d, file={%s}", __FUNCTION__, __LINE__, __FILE__);
		#define LOGD(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
		#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__) // 定义LOGI类型
		#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
		#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__) // 定义LOGW类型
		#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__) // 定义LOGE类型
		#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__) // 定义LOGF类型
	#endif
#else
	#define LOGD(...) while(0){}
	#define LOGI(...) while(0){}
	#define LOGV(...) while(0){}
	#define LOGW(...) while(0){}
	#define LOGE(...) while(0){}
	#define LOGW(...) while(0){}
#endif

/*
 * add by zhao peng
 * 在game_defend 模块和下载模块有使用下面的宏
 * */

#define RETURN 1
#define RETURN_INT 2
#define RETURN_VOID_PTR 3

#define _JUDGE_RETFLAG_VALUE \
	if(retFlag < 0){\
		LOGE  ("[-][dowhile] %s:%d ,Error, retFlag=%d, errorInfo=%s\n", __FUNCTION__, lineNum, retFlag, strerror(errno));\
	}else{\
		LOGW("[+][dowhile] %s(), Success, retFlag=%d\n", __FUNCTION__, retFlag);\
	}
#define _BREAK_DOWHILE \
	retFlag = -1*__LINE__ ;\
	lineNum = __LINE__;\
	break;
#define _FUNCSTACKINFO(x)\
	if(1){\
		if(x == 0){\
			__android_log_print(ANDROID_LOG_VERBOSE,  LOG_TAG, \
				"|--> START %s():%d, file={%s}",\
				__FUNCTION__, __LINE__, __FILE__);\
		}else{\
			__android_log_print(ANDROID_LOG_VERBOSE,  LOG_TAG, \
				"|--> END %s():%d, file={%s}",\
				__FUNCTION__, __LINE__, __FILE__);\
		}\
	}

#define _FUNC_START \
	_FUNCSTACKINFO(0)\
	int retFlag = 0, lineNum = 0;

#define _FUNC_END \
	_JUDGE_RETFLAG_VALUE\
	_FUNCSTACKINFO(1)

#define _FUNC_END_OBJ(obj) \
	_JUDGE_RETFLAG_VALUE\
	_FUNCSTACKINFO(1)\
	return obj;

#define _FUNC_END_DEFAULT \
	_JUDGE_RETFLAG_VALUE\
	_FUNCSTACKINFO(1)\
	return retFlag;

#define _FUNC_END_NULL_PTR \
	_JUDGE_RETFLAG_VALUE\
	_FUNCSTACKINFO(1)\
	return NULL;

#define _SAFE_FREE(obj)\
	if(obj != NULL){\
		free(obj);\
		obj == NULL;\
	}
#define _SAFE_CLOSE(x) \
	if(x) {\
		fclose(x);\
	}

// 创建一个size大小的type类型，并以name为名字的数组
#define CreateArrayInitWithZero(type, name, size) type ##_name[size] = {0};
#endif
