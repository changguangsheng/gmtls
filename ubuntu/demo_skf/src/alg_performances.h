#ifndef __ALG_PERFORMANCES_H__
#define __ALG_PERFORMANCES_H__

#ifndef LINUX
#include <Windows.h>
#endif
extern HANDLE hDev;
extern HAPPLICATION hApplication;

enum PerformanceAlgType
{
	P_AlG_SM1,
	P_AlG_SM4,
	P_AlG_DES,
	P_AlG_DES3_2Key,
	P_AlG_DES3_3Key,
	P_AlG_AES128,
	P_AlG_AES192,
	P_AlG_AES256,
	//add by liwy 201904
	P_AlG_SM3,
	P_AlG_SHA1,
	P_AlG_SHA256
};

enum PerformanceAlgMode
{
	P_AlG_ECB,
	P_AlG_CBC,
	P_AlG_CFB,
	P_AlG_OFB
};

#ifdef __cplusplus
extern "C" {
#endif

void alg_performances_test();

#ifdef __cplusplus
};
#endif

#endif	//__ALG_PERFORMANCES_H__
