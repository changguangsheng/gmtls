
/* �豸���ʿ��ƽӿ� */


#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_control_funs.h"



void dev_control_funs_test()
{
	ULONG rv = 0;
	HAPPLICATION hApp = NULL;
	DEVINFO devInfo;
	HCONTAINER hCon = NULL; 
	HANDLE hKey = NULL;
	BLOCKCIPHERPARAM EncryptParam;
	BYTE bAuthKey[16] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
	BYTE bRandom[16] = {0}, bAuthData[16] = {0};
	ULONG ulAuthDataLen = 16, ulRandomLen = 16 ;
	BYTE bNewAuthKey[16] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
	ULONG ulNewAuthKeyLen = 16;
	ULONG RetryCounter = 0;
	
	// �豸��֤
	// ��֤����: 
	/* 1.����֤�� ����SKF_GenRandom���豸��ȡulRandomLen(��֤�㷨һ������ĳ���)�������bRandom
	   2.����֤�� ʹ���Լ�����֤��Կ���豸��֤�㷨 ��bRandom���ܣ��õ���֤����bAuthData
	   3.����֤�� ����SKF_DevAuth()����֤����bAuthData�����豸
	   4.�豸 ʹ�ÿ��ڴ洢���豸��֤��Կ���豸��֤�㷨 ����֤����bAuthData��������D1
	   5.�豸 ����������D1��SKF_GenRandomʱ����������� �Ա�, һ����֤ͨ��,������֤ʧ��
	*/
	//==>  ����ģ�� ����֤��ʹ����֤��Կ���豸��֤�㷨 ��bRandom���� ��ȡ��֤����bAuthData����

	

	memset(&devInfo,0, sizeof(DEVINFO));
	rv = SKF_GetDevInfo(hDev, &devInfo);
	rv = SKF_GenRandom(hDev, bRandom, ulRandomLen);	
	//rv = SKF_SetSymmKey(hDev, bAuthKey, devInfo.DevAuthAlgId, &hKey);
	rv = SKF_SetSymmKey(hDev, bAuthKey, SGD_SMS4_ECB, &hKey);
	EncryptParam.PaddingType = 0;
	EncryptParam.IVLen = 0;
	memset(EncryptParam.IV,0,32);
	rv = SKF_EncryptInit(hKey, EncryptParam);
	rv = SKF_Encrypt(hKey, bRandom, ulRandomLen, bAuthData, &ulAuthDataLen);
	rv = SKF_CloseHandle(hKey);
	//<==
	rv = SKF_DevAuth(hDev, bAuthData, ulAuthDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_DevAuth Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_DevAuth OK\n");
	clear_dev_app();
	// �޸��豸��֤��Կ
	// ֻ�����豸��֤ͨ����ſ������豸��֤��Կ���޸�
	// bNewAuthKeyΪ����֤��Կ,ulNewAuthKeyLenΪ����֤��Կ����, ����֤��Կ���ȱ�����ԭ��֤��Կ����һ��
	rv = SKF_ChangeDevAuthKey(hDev, bNewAuthKey, ulNewAuthKeyLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ChangeDevAuthKey Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ChangeDevAuthKey OK\n");
	SKF_DeleteApplication(hDev, "Test_Application");
	rv = SKF_CreateApplication(hDev, "Test_Application", "111111", 3, "111111", 3, SECURE_ANYONE_ACCOUNT, &hApp);
	if (rv == SAR_OK)
	{
#if 0
		/*���Դ�����Ӧ��*/
		rv = SKF_CreateApplication(hDev, "Test_Application2", "111111", 3, "111111", 3, SECURE_ANYONE_ACCOUNT, &hApp);
		if(SAR_OK != rv)
		{
			PrintMsg("SKF_CreateApplication create the second application failed and the error code = %x",rv);
		}
#endif
		// У��PIN��(��¼Ӧ��),��ȡ��ӦȨ��
		// ����PIN����USER_TYPE/ADMIN_TYPE �Ͷ�ӦPIN��, ��¼Ӧ��,��ȡ��ӦȨ�ޣ�У��ʧ��RetryCounter����ʣ��У�����
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "111111", &RetryCounter);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_VerifyPIN Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_VerifyPIN OK\n");

		// �޸�PIN
		// У��PINͨ����ſ������޸�PIN
		// ����PIN����USER_TYPE/ADMIN_TYPE ����PIN�롢�޸ĳɵ���PIN��,�޸�ʧ��RetryCounter����ʣ�����
		rv = SKF_ChangePIN(hApp, USER_TYPE, "111111", "123456", &RetryCounter);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_ChangePIN Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_ChangePIN OK\n");
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "123456", &RetryCounter);

		// ����У��PIN��RetryCounter = 0  PIN����
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		// ����PIN
		// PIN������,ʹ�ù���ԱPIN ["111111"], �����������û���PIN["654321"],����ʧ��RetryCounter���ع���Ա�ɲ���ʣ�����
		rv = SKF_UnblockPIN(hApp,"111111", "654321", &RetryCounter);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_UnblockPIN Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_UnblockPIN OK\n");
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "654321", &RetryCounter);	//�����ɹ����У��ͨ��

		// ���Ӧ�ð�ȫ״̬
		// ���Ӧ�ð�ȫ״̬��,Ӧ�ô����޵�¼״̬
		rv = SKF_ClearSecureState(hApp);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_ClearSecureState Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_ClearSecureState OK\n",rv);
		// ��ʱ,SKF_CreateContainer�᷵���û�û�е�½0x0A00002D
		rv = SKF_CreateContainer(hApp, "TestContainer", &hCon);	
		rv = SKF_CloseApplication(hApp);
		rv = SKF_DeleteApplication(hDev, "Test_Application");
	}
}
