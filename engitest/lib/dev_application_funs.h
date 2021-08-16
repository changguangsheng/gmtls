#ifndef __DEV_APPLICATION_FUNS_H__
#define __DEV_APPLICATION_FUNS_H__
#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;
extern HAPPLICATION hApplication;


#ifdef __cplusplus
extern "C" {
#endif

void clear_dev_app();
void dev_appliction_funs_test();

#ifdef __cplusplus
};
#endif

#endif	//__DEV_APPLICATION_FUNS_H__
