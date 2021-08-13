
/* 设备管理接口 */


#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_manage_funs.h"

extern int bMultiTest;

void dev_manage_funs_test()
{
	ULONG rv = 0;
	char keylist[1280] = {0};
	ULONG keylistLen = 1280;
	char szDevName[1280] = {0};
	ULONG ulDevNameLen = 8, ulEvent = 0;
	ULONG ulDevState = 0;
	char szLabel[32] = {0};
	DEVINFO devInfo;
	ULONG ulTimeOut = 1000;


	// 枚举设备
	// TRUE 枚举当前存在的设备,keylist保存枚举到的设备名称[如I:],keylistLen返回设备长度,为设备个数*4
	rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumDev Wrong\n",rv);
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumDev OK\n",rv);

	// 连接设备
	// 输入设备名称keylist, 连接返回设备句柄hDev
	rv = SKF_ConnectDev(keylist, &hDev);
	if (rv != SAR_OK || hDev == NULL)
	{
		PrintMsg("SKF_ConnectDev Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ConnectDev OK\n");


	if (!bMultiTest)
	{
		// 等待设备拔插
		PrintMsg("请拔掉设备...\n");
		// szDevName传出发生拔插事件的设备名称[如I:], ulDevNameLen表示设备名称的有效长度, 
		// ulEvent为事件类型[1-插入,2-拔出]
		rv = SKF_WaitForDevEvent(szDevName, &ulDevNameLen, &ulEvent);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_WaitForDevEvent Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_WaitForDevEvent OK\n");
		// 获取设备
		// szDevName传入设备名称, ulDevState传出该设备当前状态[0-不存在,1-存在]
		rv = SKF_GetDevState(szDevName, &ulDevState);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_GetDevState Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_GetDevState OK, 设备[%s]存在状态[%u]\n" ,szDevName, ulDevState);

		PrintMsg("请插上设备...\n");
		rv = SKF_WaitForDevEvent(szDevName, &ulDevNameLen, &ulEvent);
		rv = SKF_GetDevState(szDevName, &ulDevState);
		rv = SKF_ConnectDev(szDevName, &hDev);
	}

	// 设置设备标签
	// szLabel为要设置的标签字串,需小于32字节
	memcpy(szLabel, "WXTSDKEY", 8);
	rv = SKF_SetLabel(hDev, szLabel);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_SetLabel Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_SetLabel OK\n");
	
	// 获取设备信息	 devInfo为保存的设备信息结构
	memset(&devInfo, 0,sizeof(DEVINFO));
	rv  = SKF_GetDevInfo(hDev, &devInfo);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_GetDevInfo Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_GetDevInfo OK\n");

	// 锁定/解锁设备 == 获取/释放设备的独占权
	rv = SKF_LockDev(hDev, ulTimeOut);	// ulTimeOut超时时间,以毫秒为单位
	rv = SKF_UnlockDev(hDev);


}
