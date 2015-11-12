/*
 * Bridge.h
 *
 *  Created on: 2015年11月12日
 *      Author: peng
 */

#ifndef BRIDGE_H_
#define BRIDGE_H_


int registerNativeMethod();

typedef struct _Bridge{
	int (*registerNativeMethod)();
}Bridge;

#endif /* BRIDGE_H_ */
