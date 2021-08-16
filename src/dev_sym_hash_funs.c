#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_sym_hash_funs.h"



void dev_sym_hash_funs_test()
{
	ULONG rv = 0;
	
	HANDLE hKey = NULL;
	BYTE bKeyValue[32] = {0x37,0xB7,0xE3,0x25,0x9C,0x36,0xE3,0x8E,0xC3,0xA5,0x85,0x86,0x10,0xb5,0xb0,0x7a};
	BYTE bIVValue[32] = {0x37,0xE3,0xA5,0x25,0xE3,0x8E,0xC3,0x85,0x86,0x9C,0x10,0xb5,0x36,0xb0,0x7a,0xB7};
	ULONG ulAlgID = 0;
	BLOCKCIPHERPARAM EncryptParam;
	BYTE* bIndata = NULL, *bOutData = NULL, *bUpdateOut = NULL, *bFinalOut = NULL;
	ULONG ulIndataLen = 0, ulOutdataLen = 0, ulUpdateOutLen = 0, ulFinalOutLen = 0;

	HANDLE hHash = NULL;

	// ************************************************
	// **************** 对称算法加解密 ****************
	// ************************************************
	/*==> 
	对称算法加解密接口使用流程: 
	   SKF_SetSymmKey明文导入会话密钥(加解密使用密钥) 
			==> SKF_EncryptInit/SKF_DecryptInit加解密初始化(设置初始IV、是否对明文数据填充)
			==> SKF_Encrypt(SKF_EncryptUpdate + SKF_EncryptFinal) / SKF_Decrypt(SKF_DecryptUpdate + SKF_DecryptFinal)对数据加解密,得到加解密结果
	<==*/
	
	// 明文导入会话密钥		
	// bKeyValue为运算用到的密钥  ulAlgID为加解密运算的算法标识  
	//		SGD_SM1_ECB/SGD_SM1_CBC/SGD_SM1_CFB/SGD_SM1_OFB/SGD_SMS4_ECB.../SGD_AES_ECB../SGD_3DES_ECB/
	ulAlgID = SGD_SM1_ECB;
	rv = SKF_SetSymmKey(hDev, bKeyValue, ulAlgID, &hKey);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_SetSymmKey Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_SetSymmKey OK\n");

	// 加密/解密 初始化
	// BLOCKCIPHERPARAM 为初始化数据结构
	/* BLOCKCIPHERPARAM 结构成员说明
	typedef struct Struct_BLOCKCIPHERPARAM{
		BYTE IV[MAX_IV_LEN];	// IV	
		ULONG IVLen;			// IV长度	ECB模式为0  需要IV时设置对应IV长度
		ULONG PaddingType;		// 是否需要软件对待加密(待解密)数据做数据填充
		ULONG FeedBitLen;		// 预留,目前可以忽略
	} BLOCKCIPHERPARAM, *PBLOCKCIPHERPARAM;
	*/
	EncryptParam.PaddingType = 0;	
	EncryptParam.IVLen = 0;
	memset(EncryptParam.IV ,0,32);
	rv = SKF_EncryptInit(hKey, EncryptParam);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EncryptInit Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EncryptInit OK\n");

	// 加密/解密	加解密使用之前必须 获得了hKey会话密钥句柄 SKF_EncryptInit初始化 
	// 单组数据加解密: bIndata为待加密数据,ulIndataLen为待加密数据长度, bOutData为返回的密文数据,ulOutdataLen为密文数据长度
	//				   bOutData为NULL时,获取需要最少存储密文数据的buffer空间大小
	ulIndataLen = 1024;	
	bIndata = (BYTE*)malloc(ulIndataLen + 1);
	if (bIndata == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	memset(bIndata, 0x22, ulIndataLen + 1);
	bOutData = (BYTE*)malloc(ulIndataLen + 1);
	if (bOutData == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	rv = SKF_Encrypt(hKey, bIndata, ulIndataLen, NULL, &ulOutdataLen);
	rv = SKF_Encrypt(hKey, bIndata, ulIndataLen, bOutData, &ulOutdataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_Encrypt Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_Encrypt OK\n");
	rv = SKF_CloseHandle(hKey);
	// 多组数据加密: SKF_EncryptUpdate与SKF_EncryptFinal必须一起使用
	rv = SKF_SetSymmKey(hDev, bKeyValue, ulAlgID, &hKey);
	rv = SKF_EncryptInit(hKey, EncryptParam);
	rv = SKF_EncryptUpdate(hKey, bIndata, ulIndataLen, NULL, &ulUpdateOutLen);
	bUpdateOut = (BYTE*)malloc(ulUpdateOutLen + 1);
	if (bUpdateOut == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	rv = SKF_EncryptUpdate(hKey, bIndata, ulIndataLen, bUpdateOut, &ulUpdateOutLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EncryptUpdate Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EncryptUpdate OK\n");
	rv = SKF_EncryptFinal(hKey, NULL, &ulFinalOutLen);
	bFinalOut = (BYTE*)malloc(ulUpdateOutLen + 1);
	if (bFinalOut == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	rv = SKF_EncryptFinal(hKey, bFinalOut, &ulFinalOutLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EncryptFinal Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EncryptFinal OK\n");
	rv = SKF_CloseHandle(hKey);
	// 多组数据加密结果由 bUpdateOut||bFinalOut 构成
	memset(bOutData, 0x00, ulIndataLen);
	memcpy(bOutData, bUpdateOut, ulUpdateOutLen);
	memcpy(bOutData + ulUpdateOutLen, bFinalOut, ulFinalOutLen);

	// 解密接口、消息鉴别码接口的使用与加密接口使用相似
	// SKF_SetSymmKey => SKF_DecryptInit => SKF_Decrypt单组数据解密【SKF_DecryptUpdate/SKF_DecryptFinal多组数据解密】
	// SKF_SetSymmKey => SKF_MacInit => SKF_Mac单组数据【SKF_MacUpdate/SKF_MacFinal多组数据】



	// ************************************************
	// **************** 杂凑(哈希)运算 ****************
	// ************************************************
	// 算法标识 SGD_SM3/SGD_SHA1/SGD_SHA256
	/*==> 接口使用流程: 
	  SKF_DigestInit杂凑初始化 ==> SKF_Digest单组数据杂凑/SKF_DigestUpdate + SKF_DigestFinal多组数据杂凑
	<==*/

	// 杂凑初始化
	// 设置杂凑算法标识ulAlgID, 获得杂凑运算句柄hHash
	// NULL, NULL, 0处参数分别代表 签名者公钥、签名者ID、签名者ID长度,  这三个参数只在SGD_SM3算法 签名者ID存在情况时使用
	ulAlgID = SGD_SHA1;
	rv = SKF_DigestInit(hDev, ulAlgID, NULL, NULL, 0, &hHash);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_DigestInit Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_DigestInit OK\n");

	// 单组数据杂凑
	// 使用之前必须使用SKF_DigestInit
	// bIndata 为待杂凑运算数据, ulIndataLen为长度, bOutData为杂凑结果, ulOutdataLen为杂凑结果长度
	ulOutdataLen = 1024;
	memset(bOutData, 0x00, 1024);
	rv = SKF_Digest(hHash, bIndata, ulIndataLen, bOutData, &ulOutdataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_Digest Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_Digest OK\n");
	rv = SKF_CloseHandle(hHash);
	// 多组数据杂凑
	// SKF_DigestUpdate/SKF_DigestFinal必须一起使用，之前必须SKF_DigestInit
	// SKF_DigestUpdate对多个分组进行杂凑计算, SKF_DigestFinal结束杂凑并获得杂凑结果
	rv = SKF_DigestInit(hDev, ulAlgID, NULL, NULL, 0, &hHash);
	rv = SKF_DigestUpdate(hHash, bIndata, ulIndataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_DigestUpdate Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_DigestUpdate OK\n");
	rv = SKF_DigestFinal(hHash, bOutData, &ulOutdataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_DigestFinal Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_DigestFinal OK\n");
	rv = SKF_CloseHandle(hHash);


	if (bIndata != NULL)
	{
		free(bIndata);
		bIndata = NULL;
	}
	if (bOutData != NULL)
	{
		free(bOutData);
		bOutData = NULL;
	}
	if (bUpdateOut != NULL)
	{
		free(bUpdateOut);
		bUpdateOut = NULL;
	}
	if (bFinalOut != NULL)
	{
		free(bFinalOut);
		bFinalOut = NULL;
	}
}