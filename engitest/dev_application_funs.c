
/* 应用接口 */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_application_funs.h"



void dev_appliction_funs_test()
{
    ULONG rv = 0;
    char AppName[256] = {0};
    ULONG ulSize = 256;

    // 创建应用
    // Test_Application为应用名称, 应用名称小于64字节
    // "123456"为管理员PIN码,5为管理员PIN码最大重试次数, "654321", 3为用户PIN码和最大重试次数,PIN码长度为6-16字节
    // SECURE_ANYONE_ACCOUNT为在该应用下创建文件/容器的权限：
    // SECURE_ANYONE_ACCOUNT任何人都可以/SECURE_NEVER_ACCOUNT任何人都不能/SECURE_ADM_ACCOUNT只管理员可以/SECURE_USER_ACCOUNT用户
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

    // 枚举应用
    // 枚举当前设备中的应用,AppName保存枚举到的应用名称,应用名称间以'\0'分割,ulSize为所有应用名称所占空间大小
    rv = SKF_EnumApplication(hDev, AppName, &ulSize);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_EnumApplication Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_EnumApplication OK\n");

    // 打开应用
    // 打开名字为AppName的应用, 打开成功获得应用句柄hApplication
    rv = SKF_OpenApplication(hDev, AppName, &hApplication);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_OpenApplication Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_OpenApplication OK\n");

    // 关闭应用
    // 关闭应用句柄指向的应用
    rv = SKF_CloseApplication(hApplication);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_CloseApplication Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_CloseApplication OK\n");


    // 删除应用
    // 删除名字为Application的应用,如果应用存在则删除成功,否则返回应用不存在错误码,因为无Application会删除失败，以下实现只供参考。
    rv = SKF_DeleteApplication(hDev, "Application");
    PrintMsg("SKF_DeleteApplication OK\n");
}


// 清空设备中的应用
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