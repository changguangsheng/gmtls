
/* 容器接口 */

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
    // 创建容器
    // 传入创建的容器名称Test_Container, 创建成功得到容器句柄hContainer
    // 容器名称长度小于64字节, 不能创建名字相同的容器,名字不能为""
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

    // 枚举容器
    // 枚举出应用中的所有容器,容器名称以'\0'分割,ulNameListSize为所有名称所占空间大小
    rv = SKF_EnumContainer(hApplication, szContainerNameList, &ulNameListSize);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_EnumContainer Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_EnumContainer OK\n");

    // 打开容器
    // 打开名称为szContainerName的应用，如果应用存在打开成功获得容器句柄,不存在则打开失败
    rv = SKF_OpenContainer(hApplication, szContainerName, &hContainer);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_OpenContainer Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_OpenContainer OK\n");


    // 获取容器类型
    // ulContainerType返回当前容器类型, 0-尚未分配类型或空容器/1-RSA容器/2-ECC容器
    // 容器类型在 容器中导入/生成RSA(ECC)签名密钥对之后,发生改变
    rv = SKF_GetContainerType(hContainer, &ulContainerType);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_GetContainerType Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_GetContainerType[%u] OK\n", ulContainerType);

    // 导入/导出数字证书
    // bSignflag为FALSE时证书为加密证书, TRUE时为签名证书, 证书最大长度为2044字节。
    ulCerTextLen = 1024;    //证书长度大小，假设证书长度是1024字节
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

    // 关闭容器
    // 关闭句柄为hContainer的容器, 关闭后hContainer无效
    rv = SKF_CloseContainer(hContainer);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_CloseContainer Wrong\n");
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("SKF_CloseContainer OK\n");

    // 删除容器
    rv = SKF_DeleteContainer(hApplication, "ContainerName");
    PrintMsg("SKF_DeleteContainer OK\n");
}