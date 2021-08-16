
/* Ӧ�ýӿ� */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_application_funs.h"



void dev_appliction_funs_test()
{
	ULONG rv = 0;
	char AppName[256] = {0};
	ULONG ulSize = 256;
		
	// ����Ӧ��
	// Test_ApplicationΪӦ������, Ӧ������С��64�ֽ�
	// "123456"Ϊ����ԱPIN��,5Ϊ����ԱPIN��������Դ���, "654321", 3Ϊ�û�PIN���������Դ���,PIN�볤��Ϊ6-16�ֽ�
	// SECURE_ANYONE_ACCOUNTΪ�ڸ�Ӧ���´����ļ�/������Ȩ�ޣ�
	// SECURE_ANYONE_ACCOUNT�κ��˶�����/SECURE_NEVER_ACCOUNT�κ��˶�����/SECURE_ADM_ACCOUNTֻ����Ա����/SECURE_USER_ACCOUNT�û�
	rv = SKF_CreateApplication(hDev, "Test_Application", "123456", 5, "654321", 3, SECURE_ANYONE_ACCOUNT, &hApplication);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CreateApplication Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	rv = SKF_CloseApplication(hApplication);
	PrintMsg("SKF_CreateApplication OK\n");

	// ö��Ӧ��
	// ö�ٵ�ǰ�豸�е�Ӧ��,AppName����ö�ٵ���Ӧ������,Ӧ�����Ƽ���'\0'�ָ�,ulSizeΪ����Ӧ��������ռ�ռ��С
	rv = SKF_EnumApplication(hDev, AppName, &ulSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumApplication Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumApplication OK\n");

	// ��Ӧ��
	// ������ΪAppName��Ӧ��, �򿪳ɹ����Ӧ�þ��hApplication
	rv = SKF_OpenApplication(hDev, AppName, &hApplication);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_OpenApplication Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_OpenApplication OK\n");

	// �ر�Ӧ��
	// �ر�Ӧ�þ��ָ���Ӧ��
	rv = SKF_CloseApplication(hApplication);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CloseApplication Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_CloseApplication OK\n");


	// ɾ��Ӧ��
	// ɾ������ΪApplication��Ӧ��,���Ӧ�ô�����ɾ���ɹ�,���򷵻�Ӧ�ò����ڴ�����,��Ϊ��Application��ɾ��ʧ�ܣ�����ʵ��ֻ���ο���
	rv = SKF_DeleteApplication(hDev, "Application");
	PrintMsg("SKF_DeleteApplication OK\n");
}


// ����豸�е�Ӧ��
void clear_dev_app()
{
	DWORD rv = 0;
	int i = 0;
	char AppName[256] = {0};
	ULONG AppSize = 256;
	rv = SKF_EnumApplication(hDev,AppName,&AppSize);
	if (rv == SAR_OK)
	{
		while (i < (int)AppSize)
		{
			if (AppName[i] == '\0')
			{
				break;
			}
			rv = SKF_DeleteApplication(hDev, (char*)&(AppName[i]));
			while(AppName[i] != '\0')
			{	
				i++;
			}
			i++;
			if (AppName[i] == '\0')
			{
				break;
			}
		}
	}
}