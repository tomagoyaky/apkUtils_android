/*
 * libdex.h
 *
 *  Created on: 2015年11月6日
 *      Author: peng
 */

#ifndef MODEL_DEX_LIBDEX_H_
#define MODEL_DEX_LIBDEX_H_

int parse();

typedef struct _Dex{
	int (*parse)();
}Dex;



#endif /* MODEL_DEX_LIBDEX_H_ */
