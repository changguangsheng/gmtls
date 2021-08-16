#ifndef __DEV_CONTROL_FUNS_H__
#define __DEV_CONTROL_FUNS_H__
#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;

#ifdef __cplusplus
extern "C" {
#endif

void dev_control_funs_test();

#ifdef __cplusplus
};
#endif

#endif	//__DEV_CONTROL_FUNS_H__
