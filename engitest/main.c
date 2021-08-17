#include <stdio.h>
#include <stdarg.h>
#include "skf.h"
#include "CommonType.h"
#include "dev_manage_funs.h"
#include "dev_control_funs.h"
#include "dev_application_funs.h"
#include "dev_file_funs.h"
#include "dev_container_funs.h"




void PrintMsg( char *fmt, ... )
{
    FILE *fp = NULL;
    va_list ap;
    fp = fopen("./Demo_log.txt", "ab");
    if (fp == NULL){
        return ;
    }
    // 打印到log文件
    //fprintf(fp, "%s", "     ");
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    // 打印到窗口
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    fclose(fp);
}


void test_all_skf(){
    UINT32 lLoopNum = 1, i = 0;
    char c = 1;

#ifndef LINUX
    remove("Demo_log.txt");
#endif
    for (i = 0; i < lLoopNum; ++i){
        //Android 64位必须这么打印，否则有问题
        PrintMsg("/**********************");
        PrintMsg("%d",i+1);
        PrintMsg(" test******************************/\n");
        PrintMsg("===============================\n");
        PrintMsg("设备管理接口");
        //        设备管理接口
        //        获取设备信息
        dev_manage_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("被认证方使用认证密钥、设备认证算法 对bRandom加密 获取认证数据bAuthData过程");
        dev_control_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("创建应用");
        dev_appliction_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("操作文件/容器");
        dev_file_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("创建容器");
        dev_container_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("对称算法加解密");
        dev_sym_hash_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("ECC接口");
        dev_ecc_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        PrintMsg("rsa 接口")
        dev_rsa_funs_test();
        PrintMsg("===============================\n\n");
        /*PrintMsg("===============================\n");
         *
        alg_performances_test();
        PrintMsg("===============================\n\n");*/

        SKF_DeleteContainer(hApplication, "Test_Container");
        SKF_CloseApplication(hApplication);
        SKF_DeleteApplication(hDev, "Test_Application");

        PrintMsg("===============================\n");
        dev_skf_expand_interafce_test();
        PrintMsg("===============================\n\n");

        SKF_DisConnectDev(hDev);
        PrintMsg("===============================\n");
        EccGenerateKeyInterfaceTest();
        PrintMsg("===============================\n\n");
    }
    PrintMsg("Test complete\n");
#ifndef LINUX
    system("pause");
#endif
}


int main(){
    UINT32 lLoopNum = 1, i = 0;
    BYTE c;

    SKFE_IssueCard();
    test_all_skf();


#ifndef LINUX
    system("pause");
#endif
    return 0;
}

