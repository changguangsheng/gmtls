#ifndef __DEV_SYM_HASH_FUNS_H__
#define __DEV_SYM_HASH_FUNS_H__
#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;
extern HAPPLICATION hApplication;
extern HCONTAINER hContainer;

#ifdef __cplusplus
extern "C" {
#endif

void dev_sym_hash_funs_test();

#ifdef __cplusplus
};
#endif

#endif	//__DEV_SYM_HASH_FUNS_H__
