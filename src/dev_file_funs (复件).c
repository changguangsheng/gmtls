
/* 文件接口 */

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

	// 操作文件/容器需要先登录应用
	rv = SKF_OpenApplication(hDev, "Test_Application", &hApplication);
	rv = SKF_VerifyPIN(hApplication, USER_TYPE, "654321" ,&ulRetryCounter);

	// 创建文件
	// Test_File为文件名, 长度小于32字节;  ulFileSize为文件大小,  该名称的文件存在时创建失败
	// SECURE_ANYONE_ACCOUNT处参数为 文件读权限,SECURE_ANYONE_ACCOUNT任何人可读/SECURE_ADM_ACCOUNT只管理员可读/SECURE_USER_ACCOUNT只用户可读
	// SECURE_USER_ACCOUNT处参数为 文件写权限, SECURE_ANYONE_ACCOUNT任何人可写/SECURE_ADM_ACCOUNT只管理员可写/SECURE_USER_ACCOUNT只用户可写
	rv = SKF_CreateFile(hApplication, "Test_File", ulFileSize, SECURE_ANYONE_ACCOUNT, SECURE_USER_ACCOUNT);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_CreateFile Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_CreateFile OK\n");

	// 枚举文件
	// 枚举出应用中的所有文件,文件名称以'\0'分割,ulFileListSize为所有名称所占空间大小
	rv = SKF_EnumFiles(hApplication, szFileList, &ulFileListSize);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_EnumFiles Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_EnumFiles OK\n");

	// 获取文件信息
	// FileInfo为存放文件信息的结构体, 其中包含文件名称、大小、读写权限
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

	// 写文件
	// 向名字为Test_File的文件中写入ulDataSize长度bData数据,ulOffset为写入文件位置的偏移量
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

	// 读文件
	// 从文件ulOffset偏移处读取ulDataSize长度的数据到bData, 最后参数ulDataSize表示实际读取到的长度
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

	// 删除文件
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