#include <stdio.h>
#include <stdarg.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_manage_funs.h"
#include "dev_control_funs.h"
#include "dev_application_funs.h"
#include "dev_container_funs.h"
#include "dev_file_funs.h"
#include "dev_sym_hash_funs.h"
#include "dev_ecc_funs.h"
#include "dev_rsa_funs.h"
#include "alg_performances.h"
#include "dev_skf_expand_interface.h"
#include "test_update_firmware.h"


#define DEMO_SKF_VERISION	"2.6"

HANDLE hDev = NULL;
HAPPLICATION hApplication = NULL;
HCONTAINER hContainer = NULL;
int bMultiTest = 1;

// 标准数据 这是ECC密钥协商所需的标准数据
unsigned char X_sm2[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x3C,0xB5,0x72,0xF0,0xD5,0x48,0xD4,0x89,0xA8,0xDB,0x97,0xB7,0x37,0xE0,0xAC,0x63,
        0x36,0x7F,0x10,0xE8,0x06,0xD1,0xFA,0xF3,0x1D,0x7C,0x0D,0x09,0xEF,0xBF,0x82,0x4F
};
unsigned char Y_sm2[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0xA2,0x77,0x85,0x1E,0xB1,0x1B,0x88,0xF5,0x97,0xCA,0xFB,0xBF,0x87,0xF8,0x62,0x55,
        0xFA,0xC4,0xE5,0xE0,0x21,0xF9,0xE6,0x5B,0x02,0xEA,0x5C,0xEA,0x78,0x10,0xBD,0x4A
};
unsigned char prikey_sm2[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x5B,0x9A,0x69,0xF5,0x9B,0xA5,0xC1,0xB5,0xFA,0x88,0x7E,0x2D,0x32,0xE9,0xA0,0xA4,
        0x31,0x77,0x5C,0xB2,0xB1,0xFD,0xC1,0x31,0x78,0x29,0x67,0xF2,0xBE,0x38,0x08,0xA3
};

void PrintMsg( char *fmt, ... );
// 密钥协商所需的连接设备和设备认证接口
/*
输入：
uzDevName:设备盘符
输出：
phDev：设备句柄
*/
int ConnectAndAuthDev(char* uzDevName, HANDLE* phDev);
// ECC密钥协商测试
/*
无
*/
void EccGenerateKeyInterfaceTest();
void get_hw_sw_info();
void get_hw_code();
void test_all_skf();
void test_alg_performances_skf();

void clearInputBuffer()
{
    int temp;
    if(scanf("%d",&temp) != EOF)
    {
        while((temp=getchar()) != '\n' && temp != EOF)
        {
            ;
        }
    }
}

int main()
{
    UINT32 lLoopNum = 1, i = 0;
    BYTE c;

    SKFE_IssueCard();

    get_hw_sw_info();

    RETRY:
    printf("1. Basic SKF interface test \n");
    printf("2. Firmware update test \n");
    printf("3. Get hardware code \n");
    printf("4. alg performances test \n");
    PrintMsg("> pls select test item : \n");
    fflush(stdin);
    scanf("%d",&c);

    if(0x02 == c)
    {
        test_update_firmware();

    }
    else if(0x01 == c)
    {
        test_all_skf();
    }
    else if(0x03 == c)
    {
        get_hw_code();
    }
    else if(0x04 == c)
    {
        test_alg_performances_skf();
    }
    else
    {
        printf("Selected test item invalid \n");
    }
    goto RETRY;
    
#ifndef LINUX
    system("pause");
#endif 
    return 0;
}

//get some software & hardware version
void get_hw_sw_info()
{
    ULONG rv;
    char keylist[256] = {0};
    ULONG keylistLen = 256;
    char szDevName[256] = {0};
    ULONG ulDevNameLen = 8, ulEvent = 0;
    DEVINFO di;
    PRODUCTINFO pi;
    ULONG ulPlatformType = 0x00;

    //enum device
    rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
    if (rv != SAR_OK)
    {
        printf("SKF_EnumDev Wrong 0x%08X\n",rv);
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_EnumDev OK\n",rv);

    //connect
    rv = SKF_ConnectDev(keylist, &hDev);
    if (rv != SAR_OK || hDev == NULL)
    {
        printf("SKF_ConnectDev Wrong\n");
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_ConnectDev OK\n");

    //get platform type
    rv = SKFE_GetPlatformType(hDev, &ulPlatformType);
    if (rv != SAR_OK || hDev == NULL)
    {
        printf("SKFE_GetPlatformType Wrong\n");
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKFE_GetPlatformType OK. platform = 0x%02X\n",ulPlatformType);
	
    memset(&di,0x00,sizeof(DEVINFO));
    rv = SKF_GetDevInfo(hDev, &di);
    if (SAR_OK != rv)
    {
        printf("SKF_GetDevInfo Wrong errcode = %08X\n",rv);
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_GetDevInfo OK\n");

    memset(&pi,0x00,sizeof(PRODUCTINFO));
    rv = SKFE_GetProductInfo(hDev, &pi);
    if (SAR_OK != rv)
    {
        printf("SKF_GetProductinfo Wrong\n");
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_GetProductinfo OK\n");

    SKF_DisConnectDev(hDev);
	
    //print info
    printf("\n****************************************************************\n");
    PrintMsg("DemoSKF version:	%s \n", DEMO_SKF_VERISION);
    PrintMsg("Firmware Version:	%d.%d \n",di.FirmwareVersion.major,di.FirmwareVersion.minor);
    PrintMsg("SKF Build Version:	%s \n", pi.SKFVer);
    printf("****************************************************************\n\n");
	
}

void get_hw_code()
{
    ULONG rv;
    BYTE ucHwCode[64] = {0};
    ULONG ulHwCodeLen = 64;
    char keylist[256] = {0};
    ULONG keylistLen = 256;
    char szDevName[256] = {0};
    ULONG ulDevNameLen = 8, ulEvent = 0;

    //enum device
    rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
    if (rv != SAR_OK)
    {
        printf("SKF_EnumDev Wrong 0x%08X\n",rv);
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_EnumDev OK\n",rv);

    //connect
    rv = SKF_ConnectDev(keylist, &hDev);
    if (rv != SAR_OK || hDev == NULL)
    {
        printf("SKF_ConnectDev Wrong\n");
        fflush(stdin);
        getchar();
        return;
    }
    printf("SKF_ConnectDev OK\n");
    //get hardware encode
    rv = SKFE_GetHwCode(hDev, ucHwCode,&ulHwCodeLen);
    if(SAR_OK != rv)
    {
        printf("SKFE_GetHwCode Wrong : 0x%08X\n",rv);
        fflush(stdin);
        getchar();
        return;
    }
    SKF_DisConnectDev(hDev);

    //print info
    PrintMsg("\n****************************************************************\n");
    PrintMsg("Hardware Version:	%s \n",ucHwCode);
    PrintMsg("****************************************************************\n\n");
}

void test_all_skf()
{
    UINT32 lLoopNum = 1, i = 0;
    char c = 1;

#ifndef LINUX
    remove("Demo_log.txt");
#endif 
    for (i = 0; i < lLoopNum; ++i)
    {
        //Android 64位必须这么打印，否则有问题
        PrintMsg("/**********************");
        PrintMsg("%d",i+1);
        PrintMsg(" test******************************/\n");
        PrintMsg("===============================\n");
        dev_manage_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_control_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_appliction_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_file_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_container_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_sym_hash_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_ecc_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_rsa_funs_test();
        PrintMsg("===============================\n\n");
        /*PrintMsg("===============================\n");
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

void test_alg_performances_skf()
{
    UINT32 lLoopNum = 1, i = 0;
    char c = 1;

#ifndef LINUX
    remove("Demo_log.txt");
#endif 
    for (i = 0; i < lLoopNum; ++i)
    {
        //Android 64位必须这么打印，否则有问题
        PrintMsg("/**********************");
        PrintMsg("%d",i+1);
        PrintMsg(" test******************************/\n");
        PrintMsg("===============================\n");
        dev_manage_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_control_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_appliction_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_file_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_container_funs_test();
        PrintMsg("===============================\n\n");
        /*PrintMsg("===============================\n");
        dev_sym_hash_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_ecc_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_rsa_funs_test();
        PrintMsg("===============================\n\n");*/
        PrintMsg("===============================\n");
        alg_performances_test();
        PrintMsg("===============================\n\n");

        SKF_DeleteContainer(hApplication, "Test_Container");
        SKF_CloseApplication(hApplication);
        SKF_DeleteApplication(hDev, "Test_Application");

        SKF_DisConnectDev(hDev);
    }
    PrintMsg("Test complete\n");
#ifndef LINUX
    system("pause");
#endif 
}

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

// 连接 认证设备
int ConnectAndAuthDev(char* uzDevName, HANDLE* phDev)
{
    DWORD rv = 0;
    UCHAR KeyValue[16] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
    HANDLE hDev = NULL;
    DEVINFO dev_info;
    BYTE ucRandom[16] = {0};
    DWORD ulRandomLen = 16;
    HANDLE   hImportSessionKey = NULL;
    BLOCKCIPHERPARAM EncryptParam;
    UCHAR ucDevauth[16] = {0};
    ULONG ulDevAuth = 16;

    rv = SKF_ConnectDev(uzDevName, &hDev);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_ConnectDev Wrong.\n");
        return -1;
    }
    *phDev = hDev;

    rv  = SKF_GetDevInfo(hDev, &dev_info);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_GetDevInfo Wrong.\n");
        return -1;
    }

    rv = SKF_GenRandom(hDev,ucRandom,ulRandomLen);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_GenRandom Wrong.\n");
        return -1;
    }

    rv = SKF_SetSymmKey(hDev,(UCHAR*)KeyValue,dev_info.DevAuthAlgId,&hImportSessionKey);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_SetSymmKey Wrong.\n");
        return -1;
    }

    EncryptParam.PaddingType = 0;
    EncryptParam.IVLen = 0;
    memset(EncryptParam.IV,0,32);
    rv = SKF_EncryptInit(hImportSessionKey,EncryptParam);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_EncryptInit Wrong.\n");
        SKF_CloseHandle(hImportSessionKey);
        return -1;
    }

    rv = SKF_Encrypt(hImportSessionKey,ucRandom,sizeof(ucRandom),NULL,&ulDevAuth);
    rv = SKF_Encrypt(hImportSessionKey,ucRandom,sizeof(ucRandom),ucDevauth,&ulDevAuth);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_Encrypt Wrong.\n");
        SKF_CloseHandle(hImportSessionKey);
        return -1;
    }
    rv = SKF_DevAuth(hDev,ucDevauth,ulDevAuth);
    if (SAR_OK != rv)
    {
        PrintMsg("SKF_DevAuth Wrong.\n");
        SKF_CloseHandle(hImportSessionKey);
        return -1;
    }
    SKF_CloseHandle(hImportSessionKey);
    return 0;
}

// 密钥协商测试
void EccGenerateKeyInterfaceTest()
{
    DWORD rv = 0;
    HANDLE hDev1 = NULL;

    char* p = NULL;
    char uzDevList[1280] = {0};
    ULONG uDevListLen = 1280;

    HAPPLICATION hApp1 = NULL;
    ULONG ulRetryTime = 0;
    HCONTAINER hContainer1 = NULL;
    ECCPUBLICKEYBLOB tSm2SignPubkeyBlob1 = {0}, tSm2EncryptPubkeyBlob1 = {0}, tSm2TmpPubkeyBlob1 = {0};
    ECCPUBLICKEYBLOB tSm2EncryptPubkeyBlob2 = {0}, tSm2TmpPubkeyBlob2 = {0};

    BYTE SymKey[16] = {0xAF,0x72,0xF6,0x68,0x95,0x6B,0xC2,0x98,0x73,0xD9,0x85,0x3A,0x4F,0xAB,0x12,0x28};
    ECCCIPHERBLOB* SymKeyCiper = (PECCCIPHERBLOB)malloc(5000);
    HANDLE hKHandle = NULL;
    BLOCKCIPHERPARAM EncParam = {0};
    BYTE EncData[256] = {0};
    ULONG EncDataLen = 256;
    ENVELOPEDKEYBLOB* EnvdKeyBlob = (PENVELOPEDKEYBLOB)malloc(5000);

    HANDLE hAgreementHandle1 = NULL;

    HANDLE hAgreementKeyHandle1 = NULL, hAgreementKeyHandle2 = NULL;

    // 以下是ECC密钥协商模拟流程（发起方和响应方是同一个设备）

    // 发起方发起ECC密钥协商流程，产生协商数据
    // 响应方使用上述协商数据也产生一个协商数据并同时产生会话密钥
    // 发起方使用响应方产生的协商数据并同时产生与响应方一样的会话密钥

    rv = SKF_EnumDev(TRUE, uzDevList, &uDevListLen);

    // 设备
    p = uzDevList;
    // 连接设备并设备认证
    if (ConnectAndAuthDev(p, &hDev1) != 0)
    {
        PrintMsg("ConnectAndAuthDev Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    // 
    hDev = hDev1; // clear_dev_app中里用的设备句柄是全局变量，故将设备的设备句柄赋值给它
    // 清除设备里所有的应用
    clear_dev_app();
    // 在设备里创建ECCTestApp应用
    rv = SKF_CreateApplication(hDev1, "ECCTestApp", "AdminPin", 5, "UserPin", 3, SECURE_ANYONE_ACCOUNT, &hApp1);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_CreateApplication Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    // 在设备里的ECCTestApp应用下进行用户PIN码认证
    rv = SKF_VerifyPIN(hApp1, USER_TYPE, "UserPin", &ulRetryTime);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_VerifyPIN Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }

    // 在设备里的ECCTestApp应用下创建ECCTestContainer容器
    rv = SKF_CreateContainer(hApp1, "ECCTestContainer", &hContainer1);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_CreateContainer Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }

    // 在设备里产生ECC公私钥对并输出公钥
    rv = SKF_GenECCKeyPair(hContainer1, SGD_SM2_1, &tSm2SignPubkeyBlob1);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_GenECCKeyPair Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    memset(SymKeyCiper, 0x00, sizeof(BYTE)*5000);
    // 使用上述产生的公钥进行ECC加密16字节数据
    rv = SKF_ExtECCEncrypt(hDev1,&tSm2SignPubkeyBlob1,SymKey,16,SymKeyCiper);

    // 加密SM2私钥

    // 设置SM1 ECB模式的会话密钥
    rv = SKF_SetSymmKey(hDev1,SymKey,SGD_SM1_ECB,&hKHandle);
    EncParam.IVLen = 0;
    EncParam.PaddingType = 0;
    rv = SKF_EncryptInit(hKHandle,EncParam);
    rv = SKF_Encrypt(hKHandle, prikey_sm2+32, 32, EncData, &EncDataLen);
    SKF_CloseHandle(hKHandle);
    // 保存加密公钥
    tSm2EncryptPubkeyBlob1.BitLen = 256;    
    memcpy(tSm2EncryptPubkeyBlob1.XCoordinate,X_sm2,sizeof(X_sm2));
    memcpy(tSm2EncryptPubkeyBlob1.YCoordinate,Y_sm2,sizeof(Y_sm2));

    // 赋值需要的结构体,导入加解密密钥对
    EnvdKeyBlob->ulBits = 256;
    EnvdKeyBlob->ulSymmAlgID = SGD_SM1_ECB;
    EnvdKeyBlob->PubKey = tSm2EncryptPubkeyBlob1;
    memcpy(&(EnvdKeyBlob->ECCCipherBlob), SymKeyCiper, sizeof(ECCCIPHERBLOB) + SymKeyCiper->CipherLen + 1);
    memcpy(EnvdKeyBlob->cbEncryptedPriKey+32, EncData, EncDataLen);

    // 导入ECC公私钥对 保护私钥的密钥算法是SM1算法
    rv = SKF_ImportECCKeyPair(hContainer1,EnvdKeyBlob);
    if (rv != SAR_OK)
    {
        PrintMsg("SKF_ImportECCKeyPair Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    // 发起方生成ECC协商数据
    rv = SKF_GenerateAgreementDataWithECC(hContainer1, SGD_SM1_ECB, &tSm2TmpPubkeyBlob1, (BYTE*)"UserID1", strlen("UserID1"), &hAgreementHandle1);
    if (rv == SAR_NOTSUPPORTYETERR)
    {
        PrintMsg("WI32U320 平台不支持 SKF_GenerateAgreementDataWithECC\n");
    }
    else if (rv != SAR_OK)
    {
        PrintMsg("SKF_GenerateAgreementDataWithECC Wrong . %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }

    // 响应方产生ECC协商数据和会话密钥
    rv = SKF_GenerateAgreementDataAndKeyWithECC(hContainer1, SGD_SM1_ECB, &tSm2EncryptPubkeyBlob1, &tSm2TmpPubkeyBlob1, &tSm2TmpPubkeyBlob2, (BYTE*)"UserID2", strlen("UserID2"), (BYTE*)"UserID1", strlen("UserID1"), &hAgreementKeyHandle2);
    if (rv == SAR_NOTSUPPORTYETERR)
    {
        PrintMsg("WI32U320 平台不支持 SKF_GenerateAgreementDataAndKeyWithECC\n");
    }
    else if (rv != SAR_OK)
    {
        PrintMsg("SKF_GenerateAgreementDataAndKeyWithECC Wrong. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    
    rv = SKF_CloseHandle(hAgreementKeyHandle2);

    // 发起方设备
    p = uzDevList;
    // 保存加密公钥
    tSm2EncryptPubkeyBlob2.BitLen = 256;    
    memcpy(tSm2EncryptPubkeyBlob2.XCoordinate,X_sm2,sizeof(X_sm2));
    memcpy(tSm2EncryptPubkeyBlob2.YCoordinate,Y_sm2,sizeof(Y_sm2));
    // 发起方产生会话密钥
    rv = SKF_GenerateKeyWithECC(hAgreementHandle1, &tSm2EncryptPubkeyBlob2, &tSm2TmpPubkeyBlob2, (BYTE*)"UserID2", strlen("UserID2"), &hAgreementKeyHandle1);
    if (rv == SAR_NOTSUPPORTYETERR)
    {
        PrintMsg("WI32U320 平台不支持 SKF_GenerateKeyWithECC\n");
    }
    else if (rv != SAR_OK)
    {
        PrintMsg("SKF_GenerateKeyWithECC. %s\n",p);
        fflush(stdin);
        getchar();
        return ;
    }
    PrintMsg("ECC Key agreement test finished.\n");
    rv = SKF_CloseHandle(hAgreementKeyHandle1);
    rv = SKF_CloseContainer(hContainer1);
    rv = SKF_CloseApplication(hApp1);
    rv = SKF_DeleteApplication(hDev1, "ECCTestApp");
    SKF_DisConnectDev(hDev1);
    free(SymKeyCiper);
    free(EnvdKeyBlob);
}