#ifndef __NATIVELOG_H
#define __NATIVELOG_H
#include <android/log.h>
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "JniHook", __VA_ARGS__))
//
#define EntryD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "Entry", __VA_ARGS__))
//

//
/*long GETLR(){
	long mLR = NULL;
	__asm__ __volatile__("mov %0, lr\n\t":  "=r"(mLR));
	return mLR;
}/**/
//

#ifdef Test
#define T_LOG(FORMAT,...) \
{ __android_log_print(ANDROID_LOG_DEBUG, "t", FORMAT, ##__VA_ARGS__); }

#else
#define T_LOG(FORMAT,...) /*\ { __android_log_print(ANDROID_LOG_DEBUG, "t", FORMAT, ##__VA_ARGS__); }*/

#endif
#endif
