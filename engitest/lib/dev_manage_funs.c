
/* �豸����ӿ� */


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


	// ö���豸
	// TRUE ö�ٵ�ǰ���ڵ��豸,keylist����ö�ٵ����豸����[��I:],keylistLen�����豸����,Ϊ�豸����*4
	rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumDev Wrong\n",rv);
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumDev OK\n",rv);

	// �����豸
	// �����豸����keylist, ���ӷ����豸���hDev
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
		// �ȴ��豸�β�
		PrintMsg("��ε��豸...\n");
		// szDevName���������β��¼����豸����[��I:], ulDevNameLen��ʾ�豸���Ƶ���Ч����, 
		// ulEventΪ�¼�����[1-����,2-�γ�]
		rv = SKF_WaitForDevEvent(szDevName, &ulDevNameLen, &ulEvent);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_WaitForDevEvent Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_WaitForDevEvent OK\n");
		// ��ȡ�豸
		// szDevName�����豸����, ulDevState�������豸��ǰ״̬[0-������,1-����]
		rv = SKF_GetDevState(szDevName, &ulDevState);
		if (rv != SAR_OK)
		{
			PrintMsg("SKF_GetDevState Wrong\n");
			fflush(stdin);
			getchar();
			return ;
		}
		PrintMsg("SKF_GetDevState OK, �豸[%s]����״̬[%u]\n" ,szDevName, ulDevState);

		PrintMsg("������豸...\n");
		rv = SKF_WaitForDevEvent(szDevName, &ulDevNameLen, &ulEvent);
		rv = SKF_GetDevState(szDevName, &ulDevState);
		rv = SKF_ConnectDev(szDevName, &hDev);
	}

	// �����豸��ǩ
	// szLabelΪҪ���õı�ǩ�ִ�,��С��32�ֽ�
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
	
	// ��ȡ�豸��Ϣ	 devInfoΪ������豸��Ϣ�ṹ
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

	// ����/�����豸 == ��ȡ/�ͷ��豸�Ķ�ռȨ
	rv = SKF_LockDev(hDev, ulTimeOut);	// ulTimeOut��ʱʱ��,�Ժ���Ϊ��λ
	rv = SKF_UnlockDev(hDev);


}
