
/* 设备访问控制接口 */


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
	
	// 设备认证
	// 认证流程: 
	/* 1.被认证方 调用SKF_GenRandom从设备获取ulRandomLen(认证算法一个分组的长度)的随机数bRandom
	   2.被认证方 使用自己的认证密钥、设备认证算法 对bRandom加密，得到认证数据bAuthData
	   3.被认证方 调用SKF_DevAuth()将认证数据bAuthData发到设备
	   4.设备 使用卡内存储的设备认证密钥、设备认证算法 对认证数据bAuthData解密数据D1
	   5.设备 将解密数据D1和SKF_GenRandom时产生的随机数 对比, 一致认证通过,否则认证失败
	*/
	//==>  以下模拟 被认证方使用认证密钥、设备认证算法 对bRandom加密 获取认证数据bAuthData过程

	

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
	// 修改设备认证密钥
	// 只有在设备认证通过后才可以做设备认证密钥的修改
	// bNewAuthKey为新认证密钥,ulNewAuthKeyLen为新认证密钥长度, 新认证密钥长度必须与原认证密钥长度一致
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
		/*测试创建多应用*/
		rv = SKF_CreateApplication(hDev, "Test_Application2", "111111", 3, "111111", 3, SECURE_ANYONE_ACCOUNT, &hApp);
		if(SAR_OK != rv)
		{
			PrintMsg("SKF_CreateApplication create the second application failed and the error code = %x",rv);
		}
#endif
		// 校验PIN码(登录应用),获取相应权限
		// 传入PIN类型USER_TYPE/ADMIN_TYPE 和对应PIN码, 登录应用,获取相应权限，校验失败RetryCounter返回剩余校验次数
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "111111", &RetryCounter);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_VerifyPIN Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_VerifyPIN OK\n");

		// 修改PIN
		// 校验PIN通过后才可以做修改PIN
		// 传入PIN类型USER_TYPE/ADMIN_TYPE 、旧PIN码、修改成的新PIN码,修改失败RetryCounter返回剩余次数
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

		// 错误校验PIN至RetryCounter = 0  PIN锁定
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "666666", &RetryCounter);
		// 解锁PIN
		// PIN锁定后,使用管理员PIN ["111111"], 解锁并设置用户新PIN["654321"],解锁失败RetryCounter返回管理员可操作剩余次数
		rv = SKF_UnblockPIN(hApp,"111111", "654321", &RetryCounter);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_UnblockPIN Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_UnblockPIN OK\n");
		rv = SKF_VerifyPIN(hApp, USER_TYPE, "654321", &RetryCounter);	//解锁成功后会校验通过

		// 清除应用安全状态
		// 清楚应用安全状态后,应用处于无登录状态
		rv = SKF_ClearSecureState(hApp);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_ClearSecureState Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_ClearSecureState OK\n",rv);
		// 此时,SKF_CreateContainer会返回用户没有登陆0x0A00002D
		rv = SKF_CreateContainer(hApp, "TestContainer", &hCon);	
		rv = SKF_CloseApplication(hApp);
		rv = SKF_DeleteApplication(hDev, "Test_Application");
	}
}
