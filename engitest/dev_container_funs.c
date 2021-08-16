
/* �����ӿ� */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_container_funs.h"



void dev_container_funs_test()
{
	ULONG rv = 0;
	char* szContainerName = "Test_Container";
	char szContainerNameList[256] = {0};
	ULONG ulNameListSize = 256;
	ULONG ulContainerType = 0;
	BYTE bCerText[2048] = {0};
	ULONG ulCerTextLen = 0;
	BOOL bSignflag = FALSE;

	SKF_DeleteContainer(hApplication, "Test_Container");
	// ��������
	// ���봴������������Test_Container, �����ɹ��õ��������hContainer
	// �������Ƴ���С��64�ֽ�, ���ܴ���������ͬ������,���ֲ���Ϊ""
	rv = SKF_CreateContainer(hApplication, "Test_Container", &hContainer);
	if (rv != SAR_OK || hContainer == NULL)
	{
		PrintMsg("SKF_CreateContainer Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_CreateContainer OK\n");
	rv = SKF_CloseContainer(hContainer);

	// ö������
	// ö�ٳ�Ӧ���е���������,����������'\0'�ָ�,ulNameListSizeΪ����������ռ�ռ��С
	rv = SKF_EnumContainer(hApplication, szContainerNameList, &ulNameListSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumContainer Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumContainer OK\n");

	// ������
	// ������ΪszContainerName��Ӧ�ã����Ӧ�ô��ڴ򿪳ɹ�����������,���������ʧ��
	rv = SKF_OpenContainer(hApplication, szContainerName, &hContainer);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_OpenContainer Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_OpenContainer OK\n");


	// ��ȡ��������
	// ulContainerType���ص�ǰ��������, 0-��δ�������ͻ������/1-RSA����/2-ECC����
	// ���������� �����е���/����RSA(ECC)ǩ����Կ��֮��,�����ı�
	rv = SKF_GetContainerType(hContainer, &ulContainerType);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_GetContainerType Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_GetContainerType[%u] OK\n", ulContainerType);

	// ����/��������֤��
	// bSignflagΪFALSEʱ֤��Ϊ����֤��, TRUEʱΪǩ��֤��, ֤����󳤶�Ϊ2044�ֽڡ�
	ulCerTextLen = 1024;    //֤�鳤�ȴ�С������֤�鳤����1024�ֽ�
	memset(bCerText, 0x02, ulCerTextLen);
	rv = SKF_ImportCertificate(hContainer, bSignflag, bCerText, ulCerTextLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportCertificate Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ImportCertificate OK\n");

	memset(bCerText, 0x00, ulCerTextLen);
	rv = SKF_ExportCertificate(hContainer, bSignflag, bCerText, &ulCerTextLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExportCertificate Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExportCertificate OK\n");

	// �ر�����
	// �رվ��ΪhContainer������, �رպ�hContainer��Ч
	rv = SKF_CloseContainer(hContainer);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CloseContainer Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_CloseContainer OK\n");

	// ɾ������
	rv = SKF_DeleteContainer(hApplication, "ContainerName");
	PrintMsg("SKF_DeleteContainer OK\n");
}