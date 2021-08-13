#ifndef __DEV_FILE_FUNS_H__
#define __DEV_FILE_FUNS_H__
#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;
extern HAPPLICATION hApplication;


#ifdef __cplusplus
extern "C" {
#endif

void dev_file_funs_test();


#ifdef __cplusplus
};
#endif

#endif	//__DEV_FILE_FUNS_H__
