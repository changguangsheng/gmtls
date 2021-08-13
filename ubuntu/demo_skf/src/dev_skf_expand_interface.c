#include <stdio.h>
#include <string.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_skf_expand_interface.h"
#include "dev_application_funs.h"

void PrintMsg( char *fmt, ... );

void dev_skf_expand_interafce_test()
{
	HAPPLICATION hApp = NULL;
	HCONTAINER hContainer = NULL;
	ULONG ulRetryTime = 0;
	ULONG rv = 0;
	CHAR SN[32] = {0};
	BYTE bFirmwareVer[37] = {0};
	BYTE byExtEncPlain[128] = {0};
	ULONG ulPlainDataLen = 0;
	BYTE bPlainData[20] = {0};
	ULONG ulFirmwareVerLen = 0;
	DEVINFO devInfo;
	ECCPUBLICKEYBLOB *pub_sm2 = (PECCPUBLICKEYBLOB)malloc(sizeof(ECCPUBLICKEYBLOB)*2);
	ECCPRIVATEKEYBLOB *pri_sm2 = (PECCPRIVATEKEYBLOB)malloc(sizeof(ECCPRIVATEKEYBLOB)*2);
	ULONG AlgId = SGD_SM2_1;
	ECCPUBLICKEYBLOB *pubBlob = (PECCPUBLICKEYBLOB)malloc(sizeof(ECCPUBLICKEYBLOB)*2);	
	ECCCIPHERBLOB *eccData = (PECCCIPHERBLOB)malloc(sizeof(ECCCIPHERBLOB)*2);
	ECCCIPHERBLOB *OutEccData = (PECCCIPHERBLOB)malloc(sizeof(ECCCIPHERBLOB)*2);

	BYTE* bFirmwareData = NULL;

	UINT nFirmwareDataLen = 0;

	char szFirmWarePath[260] = {0};
	
	// �����ֽ�SN��
	memcpy(SN,"djwhfdjshfjs",strlen("djwhfdjshfjs"));
	rv = SKFE_SetSN(hDev,SN,strlen(SN));
	if (rv == SAR_NOTSUPPORTYETERR)
	{
		PrintMsg("SKFE_SetSN A12Sƽ̨��֧��\n");
	}
	else if (rv != SAR_OK)
	{
		PrintMsg("SKFE_SetSN Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKFE_SetSN OK\n");
	}

	rv  = SKF_GetDevInfo(hDev, &devInfo);
	if (rv != SAR_OK)
	{
		
		fflush(stdin);
		getchar();
	}
	
	// �ָ���������
	rv = SKFE_SetSN(hDev,"",sizeof(""));
	if (rv == SAR_NOTSUPPORTYETERR)
	{
		PrintMsg("SKFE_SetSN A12Sƽ̨��֧��\n");
	}
	else if (rv != SAR_OK)
	{
		PrintMsg("SKFE_SetSN �ָ���������Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKFE_SetSN �ָ���������OK\n");
	}


	// ����豸�������Ӧ��
	clear_dev_app();
	// ����ECCTestAppӦ��
	rv = SKF_CreateApplication(hDev, "ECCTestApp", "AdminPin", 5, "UserPin", 3, SECURE_ANYONE_ACCOUNT, &hApp);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CreateApplication Wrong\n");
		fflush(stdin);
		getchar();
	}
	// �û�Pin����֤
	rv = SKF_VerifyPIN(hApp, USER_TYPE, "UserPin", &ulRetryTime);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_VerifyPIN Wrong\n");
		fflush(stdin);
		getchar();
	}
	// ����ECCTestContainer����
	rv = SKF_CreateContainer(hApp, "ECCTestContainer", &hContainer);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CreateContainer Wrong\n");
		fflush(stdin);
		getchar();
	}
	// �����ⲿECC��˽Կ�ԣ������
	rv = SKFE_GenExtECCKey(hDev, pri_sm2, pub_sm2);
	if (rv != SAR_OK)
	{
		PrintMsg("SKFE_GenExtECCKey Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKFE_GenExtECCKey OK\n");
	}
	// �����ڲ���˽Կ��
	rv = SKF_GenECCKeyPair(hContainer,AlgId,pubBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_GenECCKeyPair Wrong\n");
		fflush(stdin);
		getchar();
	}
#if 0
	// dwq 20181017
	// �ⲿ��Կ����
	rv = SKF_ExtECCEncrypt(hDev,pubBlob,byExtEncPlain,0x10,eccData);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCEncrypt Wrong\n");
		fflush(stdin);
		getchar();
	}
	// ��ȡ�ڲ�ǩ��˽Կ���ܺ����ݳ���
	rv = SKFE_ECCDecryptSignKey(hContainer,eccData,NULL,&ulPlainDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKFE_ECCDecryptSignKey Wrong\n");
		fflush(stdin);
		getchar();
	}
	// ��ȡ�ڲ�ǩ��˽Կ����
	rv = SKFE_ECCDecryptSignKey(hContainer,eccData,bPlainData,&ulPlainDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKFE_ECCDecryptSignKey Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKFE_ECCDecryptSignKey OK\n");
	}
#endif

	// ���ĵ���ECC��˽Կ��
	rv = SKF_ImportPlainECCKeyPair(hContainer,pri_sm2, pub_sm2,1);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportPlainECCKeyPair Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKF_ImportPlainECCKeyPair OK\n");
	}
	memset(byExtEncPlain, 0x33, 0x10);
	// �ⲿ��Կ����
	rv = SKF_ExtECCEncrypt(hDev,pub_sm2,byExtEncPlain,0x10,eccData);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCEncrypt Wrong\n");
		fflush(stdin);
		getchar();
	}
	// �����ŷ�ת��
	rv = SKFE_ECCDigitalEnvelopTransform(hDev,hContainer,eccData,pub_sm2,OutEccData);
	if (rv != SAR_OK)
	{
		PrintMsg("SKFE_ECCDigitalEnvelopTransform Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKFE_ECCDigitalEnvelopTransform OK\n");
	}

	// ��ȡ�ڲ�˽Կ���ܺ����ݳ���
	rv = SKF_ECCDecrypt(hContainer,OutEccData,NULL,&ulPlainDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCDecrypt Wrong\n");
		fflush(stdin);
		getchar();
	}
	// ��ȡ�ڲ�˽Կ����
	rv = SKF_ECCDecrypt(hContainer,OutEccData,bPlainData,&ulPlainDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCDecrypt Wrong\n");
		fflush(stdin);
		getchar();
	}
	else
	{
		PrintMsg("SKF_ECCDecrypt OK\n");
	}

	free(pub_sm2);
	free(pri_sm2);
	free(pubBlob);
	free(eccData);
	free(OutEccData);
	return;
}
