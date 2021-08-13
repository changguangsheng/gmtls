#ifndef __DEV_SKF_EXPAND_INTERFACE_H__
#define __DEV_SKF_EXPAND_INTERFACE_H__
#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;
extern HAPPLICATION hApplication;
extern HCONTAINER hContainer;

#ifdef __cplusplus
extern "C" {
#endif

	void dev_skf_expand_interafce_test();

#ifdef __cplusplus
};
#endif

#endif	//__DEV_RSA_FUNS_H__