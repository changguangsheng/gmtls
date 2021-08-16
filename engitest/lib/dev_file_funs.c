
/* �ļ��ӿ� */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_file_funs.h"



void dev_file_funs_test()
{
	ULONG rv = 0, ulRetryCounter = 0;
	ULONG ulFileSize = 0x100;
	char szFileList[256] = {0};
	ULONG ulFileListSize = 256;
	FILEATTRIBUTE FileInfo;
	BYTE bData[0x100] = {0};
	ULONG ulDataSize = 0x100, ulOffset = 0;

	// �����ļ�/������Ҫ�ȵ�¼Ӧ��
	rv = SKF_OpenApplication(hDev, "Test_Application", &hApplication);
	rv = SKF_VerifyPIN(hApplication, USER_TYPE, "654321" ,&ulRetryCounter);

	// �����ļ�
	// Test_FileΪ�ļ���, ����С��32�ֽ�;  ulFileSizeΪ�ļ���С,  �����Ƶ��ļ�����ʱ����ʧ��
	// SECURE_ANYONE_ACCOUNT������Ϊ �ļ���Ȩ��,SECURE_ANYONE_ACCOUNT�κ��˿ɶ�/SECURE_ADM_ACCOUNTֻ����Ա�ɶ�/SECURE_USER_ACCOUNTֻ�û��ɶ�
	// SECURE_USER_ACCOUNT������Ϊ �ļ�дȨ��, SECURE_ANYONE_ACCOUNT�κ��˿�д/SECURE_ADM_ACCOUNTֻ����Ա��д/SECURE_USER_ACCOUNTֻ�û���д
	rv = SKF_CreateFile(hApplication, "Test_File", ulFileSize, SECURE_ANYONE_ACCOUNT, SECURE_USER_ACCOUNT);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CreateFile Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_CreateFile OK\n");

	// ö���ļ�
	// ö�ٳ�Ӧ���е������ļ�,�ļ�������'\0'�ָ�,ulFileListSizeΪ����������ռ�ռ��С
	rv = SKF_EnumFiles(hApplication, szFileList, &ulFileListSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumFiles Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumFiles OK\n");

	// ��ȡ�ļ���Ϣ
	// FileInfoΪ����ļ���Ϣ�Ľṹ��, ���а����ļ����ơ���С����дȨ��
	memset(&FileInfo, 0,sizeof(FILEATTRIBUTE));
	rv = SKF_GetFileInfo(hApplication, "Test_File", &FileInfo);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_GetFileInfo Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_GetFileInfo OK\n");

	// д�ļ�
	// ������ΪTest_File���ļ���д��ulDataSize����bData����,ulOffsetΪд���ļ�λ�õ�ƫ����
	memset(bData, 0x22, 0x100);
	rv = SKF_WriteFile(hApplication, "Test_File", ulOffset, bData, ulDataSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_WriteFile Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_WriteFile OK\n");

	// ���ļ�
	// ���ļ�ulOffsetƫ�ƴ���ȡulDataSize���ȵ����ݵ�bData, ������ulDataSize��ʾʵ�ʶ�ȡ���ĳ���
	memset(bData, 0x00, 0x100);
	rv = SKF_ReadFile(hApplication, "Test_File", ulOffset, ulDataSize, bData, &ulDataSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ReadFile Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ReadFile OK\n");

	// ɾ���ļ�
	rv = SKF_DeleteFile(hApplication, "Test_File");
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_DeleteFile Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_DeleteFile OK\n");
}