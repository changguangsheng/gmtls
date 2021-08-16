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
	// **************** �Գ��㷨�ӽ��� ****************
	// ************************************************
	/*==> 
	�Գ��㷨�ӽ��ܽӿ�ʹ������: 
	   SKF_SetSymmKey���ĵ���Ự��Կ(�ӽ���ʹ����Կ) 
			==> SKF_EncryptInit/SKF_DecryptInit�ӽ��ܳ�ʼ��(���ó�ʼIV���Ƿ�������������)
			==> SKF_Encrypt(SKF_EncryptUpdate + SKF_EncryptFinal) / SKF_Decrypt(SKF_DecryptUpdate + SKF_DecryptFinal)�����ݼӽ���,�õ��ӽ��ܽ��
	<==*/
	
	// ���ĵ���Ự��Կ		
	// bKeyValueΪ�����õ�����Կ  ulAlgIDΪ�ӽ���������㷨��ʶ  
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

	// ����/���� ��ʼ��
	// BLOCKCIPHERPARAM Ϊ��ʼ�����ݽṹ
	/* BLOCKCIPHERPARAM �ṹ��Ա˵��
	typedef struct Struct_BLOCKCIPHERPARAM{
		BYTE IV[MAX_IV_LEN];	// IV	
		ULONG IVLen;			// IV����	ECBģʽΪ0  ��ҪIVʱ���ö�ӦIV����
		ULONG PaddingType;		// �Ƿ���Ҫ����Դ�����(������)�������������
		ULONG FeedBitLen;		// Ԥ��,Ŀǰ���Ժ���
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

	// ����/����	�ӽ���ʹ��֮ǰ���� �����hKey�Ự��Կ��� SKF_EncryptInit��ʼ�� 
	// �������ݼӽ���: bIndataΪ����������,ulIndataLenΪ���������ݳ���, bOutDataΪ���ص���������,ulOutdataLenΪ�������ݳ���
	//				   bOutDataΪNULLʱ,��ȡ��Ҫ���ٴ洢�������ݵ�buffer�ռ��С
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
	// �������ݼ���: SKF_EncryptUpdate��SKF_EncryptFinal����һ��ʹ��
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
	// �������ݼ��ܽ���� bUpdateOut||bFinalOut ����
	memset(bOutData, 0x00, ulIndataLen);
	memcpy(bOutData, bUpdateOut, ulUpdateOutLen);
	memcpy(bOutData + ulUpdateOutLen, bFinalOut, ulFinalOutLen);

	// ���ܽӿڡ���Ϣ������ӿڵ�ʹ������ܽӿ�ʹ������
	// SKF_SetSymmKey => SKF_DecryptInit => SKF_Decrypt�������ݽ��ܡ�SKF_DecryptUpdate/SKF_DecryptFinal�������ݽ��ܡ�
	// SKF_SetSymmKey => SKF_MacInit => SKF_Mac�������ݡ�SKF_MacUpdate/SKF_MacFinal�������ݡ�



	// ************************************************
	// **************** �Ӵ�(��ϣ)���� ****************
	// ************************************************
	// �㷨��ʶ SGD_SM3/SGD_SHA1/SGD_SHA256
	/*==> �ӿ�ʹ������: 
	  SKF_DigestInit�Ӵճ�ʼ�� ==> SKF_Digest���������Ӵ�/SKF_DigestUpdate + SKF_DigestFinal���������Ӵ�
	<==*/

	// �Ӵճ�ʼ��
	// �����Ӵ��㷨��ʶulAlgID, ����Ӵ�������hHash
	// NULL, NULL, 0�������ֱ���� ǩ���߹�Կ��ǩ����ID��ǩ����ID����,  ����������ֻ��SGD_SM3�㷨 ǩ����ID�������ʱʹ��
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

	// ���������Ӵ�
	// ʹ��֮ǰ����ʹ��SKF_DigestInit
	// bIndata Ϊ���Ӵ���������, ulIndataLenΪ����, bOutDataΪ�Ӵս��, ulOutdataLenΪ�Ӵս������
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
	// ���������Ӵ�
	// SKF_DigestUpdate/SKF_DigestFinal����һ��ʹ�ã�֮ǰ����SKF_DigestInit
	// SKF_DigestUpdate�Զ����������Ӵռ���, SKF_DigestFinal�����Ӵղ�����Ӵս��
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