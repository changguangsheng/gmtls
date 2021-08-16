#include <stdio.h>
#include <stdarg.h>
#include "CommonType.h"
#include "skf.h"

#define APP_NAME	"Test_Application"
#define CON_NAME_1	"Test_Container_1"
#define CON_NAME_2	"Test_Container_2"
#define FILE_NAME	"Test_File"
#define FILE_NAME "Test_File"
#define MY_SN			"Test_Sn_11111111111222222222255"
#define FILE_SIZE 80

BYTE gucFileContent[40];
DEVINFO gDevInfo;

extern int ConnectAndAuthDev(char* uzDevName, HANDLE* phDev);
extern void PrintMsg( char *fmt, ... );

int init_file_system();
int update_firmware(char *szFirmWarePath, char *szDevPath);
int verify_file_system();

/*****************************************************************************
*Desp: a simple demo to test/verify update firmware interface
*Note: the verify flow : issue -> init -> update firmware -> verify user data
*Author: wangqiang@wisectech.com.cn
*Date: 2018.12.06
******************************************************************************/

int test_update_firmware()
{
    UINT32 lLoopNum = 1, i = 0;
    char c;
    char szFirmWarePath[260] = {0};
	char szDevPath[260] = {0};

    printf(">> Do you want to do a multiple test loop ? [Y/N]\n");
    fflush(stdin);
#ifdef LINUX
	while(getchar()!=10)
	{}
#endif
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        fflush(stdin);
        printf(">> input the loop times : \n");
        scanf("%d", &lLoopNum);
    }

#ifndef LINUX
    remove("Demo_log.txt");
#endif 

    //initialize some user data(application,file,container,key pair....)
    init_file_system();

    printf("\nupdate firmware....\n");

#ifdef LINUX
	printf("pls input the device path : \n");
	scanf("%s",szDevPath);
	fflush(stdin);
#endif

    printf("pls input the firmware path : \n");
    scanf("%s",szFirmWarePath);
    fflush(stdin);

    for (i = 0; i < lLoopNum; ++i)
    {
        //Android 64bit print format
        printf("/**********************");
        printf("%d",i+1);
        printf(" test******************************/\n");

		update_firmware(szFirmWarePath,szDevPath);

        if(SAR_OK == verify_file_system())
			printf("AFTER UPDATE FIRMWARE, USER DATA CONTENT IS OK !\n\n");
		else
			printf("#### AFTER UPDATE FIRMWARE, USER DATA CONTENT IS ERROR !\n\n");
    }

	SKFE_IssueCard();

    printf("Test complete\n");
#ifndef LINUX
    system("pause");
#endif 
    return 0;
}

int init_file_system()
{
	HANDLE hDev;
	ULONG rv = 0;
	char keylist[MAX_PATH] = {0};
	ULONG keylistLen = MAX_PATH;
	char szDevName[MAX_PATH] = {0};
	ULONG ulDevNameLen = 8;
	HANDLE hApplication,hContainer;
	ULONG ulRetryCounter;
	ECCPUBLICKEYBLOB tEccPubBlob;
	DEVINFO stTempDevInfo;
	UINT i;
	BYTE ucHwCode[64] = {0};
	ULONG ulHwCodeLen = 64;
	BYTE ucFwPubKey[64] = {0};
	ULONG ulFwPubKeyLen = 64;

	printf("\ninit device ....\n");

	rv = SKFE_IssueCard();
	if(SAR_OK != rv)
	{
		printf("#### SKF_IssueCard Wrong\n",rv);
		return rv ;
	}
	printf("SKF_IssueCard OK\n",rv);

	// enum device
	rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
	if(SAR_OK != rv)
	{
		printf("#### SKF_EnumDev Wrong\n",rv);
		return rv ;
	}
	printf("SKF_EnumDev OK\n",rv);

	//connect device
	if (ConnectAndAuthDev(keylist, &hDev) != 0)
	{
		printf("#### ConnectAndAuthDev failed\n");
		return rv ;
	}

	//create application
	rv = SKF_CreateApplication(hDev, APP_NAME, "123456", 5, "654321", 3, SECURE_ANYONE_ACCOUNT, &hApplication);
	if(SAR_OK != rv)
	{
		printf("#### SKF_CreateApplication Wrong\n");
		return rv ;
	}
	SKF_CloseApplication(hApplication);
	
	printf("SKF_CreateApplication OK\n");

	//open application
	rv = SKF_OpenApplication(hDev, APP_NAME, &hApplication);
	if(SAR_OK != rv)
	{
		printf("#### SKF_OpenApplication Wrong\n");
		return  rv;
	}
	printf("SKF_OpenApplication OK\n");

	//verify pin
	rv = SKF_VerifyPIN(hApplication, USER_TYPE, "654321", &ulRetryCounter);
	if(SAR_OK != rv)
	{
		printf("#### SKF_VerifyPIN Wrong\n");
		return  rv;
	}
	printf("SKF_VerifyPIN OK\n");

	//create file
	rv = SKF_CreateFile(hApplication,FILE_NAME, FILE_SIZE,SECURE_ANYONE_ACCOUNT,SECURE_ANYONE_ACCOUNT);
	if(SAR_OK != rv)
	{
		printf("#### SKF_CreateFile Wrong\n");
		return  rv;
	}
	printf("SKF_CreateFile OK\n");

	rv = SKF_GenRandom(hDev,gucFileContent,sizeof(gucFileContent));
	if(SAR_OK != rv)
	{
		printf("#### SKF_GenRandom Wrong\n");
		return  rv;
	}
	printf("SKF_GenRandom OK\n");

	rv = SKF_WriteFile(hApplication,FILE_NAME,0,gucFileContent, sizeof(gucFileContent));
	if(SAR_OK != rv)
	{
		printf("#### SKF_WriteFile Wrong\n");
		return  rv;
	}
	printf("SKF_WriteFile OK\n");

	//create container 1
	rv = SKF_CreateContainer(hApplication, CON_NAME_1, &hContainer);
	if(SAR_OK != rv)
	{
		printf("#### SKF_CreateContainer 1 Wrong\n");
		return  rv;
	}
	printf("SKF_CreateContainer 1 OK\n");

	//generate a sm2 key pair in container 1
	rv = SKF_GenECCKeyPair(hContainer, SGD_SM2_1, &tEccPubBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_GenECCKeyPair Wrong \n");
		return rv;
	}
	printf("SKF_GenECCKeyPair OK\n");

	//close container 1
	SKF_CloseContainer(hContainer);

	//create container 2
	rv = SKF_CreateContainer(hApplication, CON_NAME_2, &hContainer);
	if(SAR_OK != rv)
	{
		printf("#### SKF_CreateContainer 2 Wrong\n");
		return  rv;
	}
	printf("SKF_CreateContainer 2 OK\n");

	//generate sm2 key pair in container 2
	rv = SKF_GenECCKeyPair(hContainer, SGD_SM2_1, &tEccPubBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_GenECCKeyPair Wrong \n");
		return rv;
	}
	printf("SKF_GenECCKeyPair OK\n");

	rv = SKFE_SetSN(hDev,"",strlen(""));
	if(SAR_OK != rv)
	{
		printf("#### SKFE_SetSN Clear SN Wrong \n");
		return rv;
	}
	printf("SKFE_SetSN Clear SN OK\n");

	//devinfo
	memset(&stTempDevInfo,0x00,sizeof(DEVINFO));
	rv = SKF_GetDevInfo(hDev, &stTempDevInfo);
	if(SAR_OK != rv)
	{
		printf("#### SKF_GetDevInfo Wrong\n");
		return  rv;
	}
	printf("SKF_GetDevInfo OK\n");

	rv = SKFE_SetSN(hDev,MY_SN,strlen(MY_SN));
	if(SAR_OK != rv)
	{
		printf("#### SKFE_SetSN Wrong \n");
		return rv;
	}
	printf("SKFE_SetSN OK\n");

	//devinfo
	memset(&gDevInfo,0x00,sizeof(DEVINFO));
	rv = SKF_GetDevInfo(hDev, &gDevInfo);
	if(SAR_OK != rv)
	{
		printf("#### SKF_GetDevInfo Wrong\n");
		return  rv;
	}
	printf("SKF_GetDevInfo OK\n");

	//get firmware public key
	rv = SKFE_GetFwPublicKey(hDev,ucFwPubKey,&ulFwPubKeyLen);
	if(SAR_OK != rv)
	{
		printf("#### SKFE_GetFwPublicKey Wrong\n");
	}
	else
	{
		printf("SKFE_GetFwPublicKey OK\n");
		for(i=0; i<ulFwPubKeyLen; i++)
		{
			printf("%02X",ucFwPubKey[i]);
			if(0x00 == ((i+1)%32))
				printf("\n");
		}
	}

	SKF_CloseContainer(hContainer);
	SKF_CloseApplication(hApplication);
	SKF_DisConnectDev(hDev);

	printf("initialize device OK\n");
	return 0;
}

int update_firmware(char *szFirmWarePath, char *szDevPath)
{
	HANDLE hDev;
	//char szFirmWarePath[1024];
	BYTE* szFirmwareData = NULL;
	UINT nFirmwareDataLen = 512*1024;
	BYTE szTmpFirmwareData[1024] = {0};
	FILE *f = NULL;
	int rv = 1;
	char *name = NULL;
#if 0
	printf("\nupdate firmware....\n");
	printf("pls input the firmware path : \n");
	scanf("%s",szFirmWarePath);
	fflush(stdin);
#endif
	name = szFirmWarePath;

	f = fopen(name,"rb");
	if (NULL == f)
	{
		printf("#### open firmware file failed.\n");
		fflush(stdin);
		getchar();
		return 1;
	}

	fseek(f,0,SEEK_END);
	nFirmwareDataLen = ftell(f);
	szFirmwareData = (BYTE*)malloc((512*1024)*sizeof(BYTE));
	memset(szFirmwareData,0x00,(512*1024)*sizeof(BYTE));
	fseek(f,0,SEEK_SET);
	rv = fread(szFirmwareData,1,nFirmwareDataLen,f);
	if (0x00 == rv)
	{
		free(szFirmwareData);
		szFirmwareData = NULL;
		printf("#### read firmware file failed. \n");
		fflush(stdin);
		getchar();
		return 1;
	}       

	if(NULL == szDevPath)
	{
		printf("#### device path is null. \n");
		fflush(stdin);
		getchar();
		return 1;
	}
	//rv = SKFE_UpdateFirmware(szFirmwareData,nFirmwareDataLen);
	//rv = SKFE_UpdateFirmware(DEF_DEVICE_PATH,szFirmwareData,nFirmwareDataLen);
	rv = SKFE_UpdateFirmware(szDevPath,szFirmwareData,nFirmwareDataLen);
	if(SAR_OK != rv)
	{
		free(szFirmwareData);
		szFirmwareData = NULL;
		printf("#### SKFE_UpdateFirmware  failed  the error code = 0x%08x\n",rv);
		fflush(stdin);
		getchar();
	}
	else
		printf("SKFE_UpdateFirmware . update firmware success !!\n");

	if (szFirmwareData != NULL)
	{
		free(szFirmwareData);
	}

	fclose(f);
	
	return 0;
}

int verify_file_system()
{
	HANDLE hDev;
	ULONG rv = 0;
	char keylist[MAX_PATH] = {0};
	ULONG keylistLen = MAX_PATH;
	char szDevName[MAX_PATH] = {0};
	ULONG ulDevNameLen = 8;
	HANDLE hApplication,hContainer;
	ULONG ulRetryCounter;
	BYTE ucFileContent[64];
	ULONG ulFileContentLen;
	BYTE ucData[19] = {0};
	ECCPUBLICKEYBLOB stEccPubBlob;
	ULONG ulLen;
	ECCSIGNATUREBLOB stEccSigBlob;
	DEVINFO stTempDevInfo;

	printf("\nverify device system ....\n");

	// enum device
	rv = SKF_EnumDev(TRUE, keylist, &keylistLen);
	if(SAR_OK != rv)
	{
		printf("#### SKF_EnumDev Wrong\n",rv);
		return rv ;
	}
	printf("SKF_EnumDev OK\n",rv);

	//connect device
	if (ConnectAndAuthDev(keylist, &hDev) != 0)
	{
		printf("#### ConnectAndAuthDev failed\n");
		return 1 ;
	}

	//open application
	rv = SKF_OpenApplication(hDev, APP_NAME, &hApplication);
	if(SAR_OK != rv)
	{
		printf("#### SKF_OpenApplication Wrong\n");
		return  rv;
	}
	printf("SKF_OpenApplication OK\n");

	//verify pin
	rv = SKF_VerifyPIN(hApplication, USER_TYPE, "654321", &ulRetryCounter);
	if(SAR_OK != rv)
	{
		printf("#### SKF_VerifyPIN Wrong\n");
		return  rv;
	}
	printf("SKF_VerifyPIN OK\n");

	//read file
	ulFileContentLen = sizeof(ucFileContent);
	rv = SKF_ReadFile(hApplication,FILE_NAME, 0x00, 40, ucFileContent, &ulFileContentLen);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ReadFile Wrong\n");
		return  rv;
	}
	printf("SKF_ReadFile OK\n");
	//compare data
	if(0x00 != memcmp(ucFileContent, gucFileContent,40))
	{
		printf("#### file content changed after update firmware....\n");
		return 1;
	}

	//open container 1
	rv = SKF_OpenContainer(hApplication, CON_NAME_1, &hContainer);
	if(SAR_OK != rv)
	{
		printf("#### SKF_OpenContainer 1 Wrong\n");
		return  rv;
	}
	printf("SKF_OpenContainer 1 OK\n");
	
	//sm2 sign data
	memset(&stEccSigBlob,0x00,sizeof(ECCSIGNATUREBLOB));
	rv = SKF_ECCSignData(hContainer, ucData, 32, &stEccSigBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ECCSignData 1 Wrong rv = 0x%08X\n",rv);
		return  rv;
	}
	printf("SKF_ECCSignData 1 OK\n");

	//export public key
	memset(&stEccPubBlob,0x00,sizeof(ECCPUBLICKEYBLOB));
	ulLen = sizeof(ECCPUBLICKEYBLOB);
	rv = SKF_ExportPublicKey(hContainer, TRUE, &stEccPubBlob, &ulLen);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ExportPublicKey 1 Wrong\n");
		return  rv;
	}
	printf("SKF_ExportPublicKey 1 OK\n");

	//sm2 verify data
	rv = SKF_ECCVerify(hDev, &stEccPubBlob, ucData, 0x20, &stEccSigBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ECCVerify 1 Wrong rv = 0x%08X\n",rv);
		return  rv;
	}
	printf("SKF_ECCVerify 1 OK\n");

	//close container 1
	SKF_CloseContainer(hContainer);

	//open container 2
	rv = SKF_OpenContainer(hApplication, CON_NAME_2, &hContainer);
	if(SAR_OK != rv)
	{
		printf("#### SKF_CreateContainer 2 Wrong\n");
		return  rv;
	}
	printf("SKF_CreateContainer 2 OK\n");

	//sm2 sign data
	memset(&stEccSigBlob,0x00,sizeof(ECCSIGNATUREBLOB));
	rv = SKF_ECCSignData(hContainer, ucData, 0x20, &stEccSigBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ECCSignData 2 Wrong\n");
		return  rv;
	}
	printf("SKF_ECCSignData 2 OK\n");

	//export public key
	memset(&stEccPubBlob,0x00,sizeof(ECCPUBLICKEYBLOB));
	ulLen = sizeof(ECCPUBLICKEYBLOB);
	rv = SKF_ExportPublicKey(hContainer, TRUE, &stEccPubBlob, &ulLen);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ExportPublicKey 2 Wrong\n");
		return  rv;
	}
	printf("SKF_ExportPublicKey 2 OK\n");

	//sm2 verify data
	rv = SKF_ECCVerify(hDev, &stEccPubBlob, ucData, 0x20, &stEccSigBlob);
	if(SAR_OK != rv)
	{
		printf("#### SKF_ECCVerify 2 Wrong\n");
		return  rv;
	}
	printf("SKF_ECCVerify 2 OK\n");

	//devinfo
	memset(&stTempDevInfo,0x00,sizeof(DEVINFO));
	rv = SKF_GetDevInfo(hDev, &stTempDevInfo);
	if(SAR_OK != rv)
	{
		printf("#### SKF_GetDevInfo Wrong\n");
		return  rv;
	}
	printf("SKF_GetDevInfo OK\n");

#if 1
#if 1
	//compare sn
	if(0x00 != memcmp(MY_SN, stTempDevInfo.SerialNumber,strlen(MY_SN)))
	{
		printf("#### SN changed after update firmware....\n");
		printf("SN (before update) < %s > changed after update firmware....\n",gDevInfo.SerialNumber);
		printf("SN (after update) < %s > changed after update firmware....\n",stTempDevInfo.SerialNumber);
		//return 1;
	}
	if(0x00 != memcmp(&(stTempDevInfo.FirmwareVersion),&(gDevInfo.FirmwareVersion),sizeof(stTempDevInfo.FirmwareVersion)))
	{
		printf("FirmwareVersion changed after update firmware....\n");
		printf("Firmware Version(before update):		%d.%d \n",gDevInfo.FirmwareVersion.major,gDevInfo.FirmwareVersion.minor);
		printf("Firmware Version(after update):		%d.%d \n",stTempDevInfo.FirmwareVersion.major,stTempDevInfo.FirmwareVersion.minor);
		//return 1;
	}
#endif
#else
	//maybe we need conform all device info data as the same as before
	if(0x00 != memcmp(&stTempDevInfo,&gDevInfo,sizeof(DEVINFO)))
	{
		printf("#### DEVINFO changed after update firmware....\n");
		return 1;
	}
#endif

	SKF_CloseContainer(hContainer);
	SKF_CloseApplication(hApplication);
	SKF_DisConnectDev(hDev);

	printf("verify device system OK\n");
	return 0;
}



