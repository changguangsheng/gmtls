#ifndef __SKF_H__
#define __SKF_H__

#ifdef WIN32
#include <Windows.h>
#define DEVAPI __stdcall
#define WINAPI __stdcall
typedef void* HANDLE;
typedef HANDLE DEVHANDLE;
typedef HANDLE HAPPLICATION;
typedef HANDLE HCONTAINER;
#else
#define __stdcall
#define _stdcall
#define DEVAPI __attribute__ ((visibility ("default")))
#define WINAPI __attribute__ ((visibility ("default")))
#endif

#define MAX_RSA_MODULUS_LEN				256
#define MAX_RSA_EXPONENT_LEN			4
#define ECC_MAX_XCOORDINATE_BITS_LEN	512
#define ECC_MAX_YCOORDINATE_BITS_LEN	512
#define ECC_MAX_MODULUS_BITS_LEN		512
#define MAX_IV_LEN						32

#ifdef __cplusplus
extern "C" {
#endif

// SKF 标准中的一些复合数据类型定义

// 版本
typedef struct Struct_Version{
	unsigned char major;						// 主版本号
	unsigned char minor;						// 次版本号
}VERSION;

// 设备信息
#pragma pack(push,1)
typedef struct Struct_DEVINFO{
	VERSION		Version;			// 版本号, 设置为1.0
	char		Manufacturer[64];	// 设备厂商信息, 以'\0'为结束符的ASCII字符串
	char		Issuer[64];			// 发行厂商信息, 以'\0'为结束符的ASCII字符串
	char		Label[32];			// 设备标签, 以'\0'为结束符的ASCII字符串
	char		SerialNumber[32];	// 序列号, 以'\0'为结束符的ASCII字符串
	VERSION		HWVersion;			// 设备硬件版本
	VERSION		FirmwareVersion;	// 设备本身固件版本
	unsigned int		AlgSymCap;			// 分组密码算法标识
	unsigned int		AlgAsymCap;			// 非对称密码算法标识
	unsigned int		AlgHashCap;			// 密码杂凑算法标识
	unsigned int		DevAuthAlgId;		// 设备认证的分组密码算法标识
	unsigned int		TotalSpace;			// 设备总空间大小
	unsigned int		FreeSpace;			// 用户可用空间大小
	unsigned int		MaxECCBufferSize;	// 能够处理的ECC加密数据大小 
	unsigned int		MaxBufferSize;      // 能够处理的分组运算和杂凑运算的数据大小
	unsigned char		Reserved[64];		// 保留扩展
}DEVINFO, *PDEVINFO;
#pragma pack(pop)

// RSA公钥数据结构
typedef struct Struct_RSAPUBLICKEYBLOB{
	unsigned int AlgID;						// 算法标识
	unsigned int BitLen;						// 算法的实际位长度,必须是8的倍数
	unsigned char Modulus[MAX_RSA_MODULUS_LEN];	// 模数N
	unsigned char PublicExponent[MAX_RSA_EXPONENT_LEN];	// 公开密钥E, 一般固定为00010001
}RSAPUBLICKEYBLOB, *PRSAPUBLICKEYBLOB;

// RSA私钥数据结构
typedef struct Struct_RSAPRIVATEKEYBLOB{
	unsigned int AlgID;									// 算法标识
	unsigned int BitLen;									// 算法的实际位长度,必须是8的倍数
	unsigned char Modulus[MAX_RSA_MODULUS_LEN];				// 模数N, 实际长度为BitLen/8
	unsigned char PublicExponent[MAX_RSA_EXPONENT_LEN];		// 公开密钥E, 一般固定为00010001
	unsigned char PrivateExponent[MAX_RSA_MODULUS_LEN];		// 私钥D, 实际长度为BitLen/8
	unsigned char Prime1[MAX_RSA_MODULUS_LEN/2];				// 素数p, 实际长度为BitLen/16 
	unsigned char Prime2[MAX_RSA_MODULUS_LEN/2];				// 素数q, 实际长度为BitLen/16 
	unsigned char Prime1Exponent[MAX_RSA_MODULUS_LEN/2];		// dp, 实际长度为BitLen/16 
	unsigned char Prime2Exponent[MAX_RSA_MODULUS_LEN/2];		// dq, 实际长度为BitLen/16
	unsigned char Coefficient[MAX_RSA_MODULUS_LEN/2];		// q模p的乘法逆元, 实际长度为BitLen/16
}RSAPRIVATEKEYBLOB, *PRSAPRIVATEKEYBLOB;

// ECC公钥数据结构
typedef struct Struct_ECCPUBLICKEYBLOB{
	unsigned int BitLen;									// 模数的实际位长度, 必须是8的倍数
	unsigned char XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8]; // 曲线上点的X坐标
	unsigned char YCoordinate[ECC_MAX_YCOORDINATE_BITS_LEN/8]; // 曲线上点的Y坐标
}ECCPUBLICKEYBLOB, *PECCPUBLICKEYBLOB;

// ECC私钥数据结构
typedef struct Struct_ECCPRIVATEKEYBLOB{
	unsigned int BitLen;									// 模数的实际位长度, 必须是8的倍数
	unsigned char PrivateKey[ECC_MAX_MODULUS_BITS_LEN/8];	// 私有密钥
}ECCPRIVATEKEYBLOB, *PECCPRIVATEKEYBLOB;


// ECC密文数据结构
typedef struct Struct_ECCCIPHERBLOB{
	unsigned char XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	unsigned char YCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	unsigned char HASH[32];				// 明文的杂凑值
	unsigned int CipherLen;			// 密文数据长度
	unsigned char Cipher[1];				// 密文数据
} ECCCIPHERBLOB, *PECCCIPHERBLOB;

// ECC签名数据结构
typedef struct Struct_ECCSIGNATUREBLOB{
	unsigned char r[ECC_MAX_XCOORDINATE_BITS_LEN/8];		// 签名结构R部分
	unsigned char s[ECC_MAX_XCOORDINATE_BITS_LEN/8];		// 签名结构S部分
} ECCSIGNATUREBLOB, *PECCSIGNATUREBLOB;

// 分组密码参数
typedef struct Struct_BLOCKCIPHERPARAM{
	unsigned char IV[MAX_IV_LEN];			// 初始向量IV
	unsigned int IVLen;					// 初始向量的实际长度
	unsigned int PaddingType;				// 填充方式, 0表示不填充, 1表示按照PKCS#5方式进行填充
	unsigned int FeedBitLen;				// 反馈值的位长度(按位计算),只针对OFB、CFB
} BLOCKCIPHERPARAM, *PBLOCKCIPHERPARAM;

// ECC加密密钥对保护结构
typedef struct SKF_ENVELOPEDKEYBLOB{
	unsigned int Version;							// 当前版本为 1
	unsigned int ulSymmAlgID;						// 对称算法标识，限定ECB模式
	unsigned int ulBits;							// 加密密钥对的密钥位长度
	unsigned char cbEncryptedPriKey[64];				// 加密密钥对私钥的密文
	ECCPUBLICKEYBLOB PubKey;				// 加密密钥对的公钥
	ECCCIPHERBLOB ECCCipherBlob;			// 用保护公钥加密的对称密钥密文。
}ENVELOPEDKEYBLOB, *PENVELOPEDKEYBLOB;

// 文件属性
typedef struct Struct_FILEATTRIBUTE{
	char FileName[32];						// 文件名
	unsigned int FileSize;							// 文件大小
	unsigned int ReadRights;						// 读取权限
	unsigned int WriteRights;						// 写入权限
} FILEATTRIBUTE, *PFILEATTRIBUTE;


// APP信息组包结构
#pragma pack(push,1)
typedef struct _APPPACKDATA
{
    unsigned short DataPackNum; // 包的数量
    unsigned int COSDataTotalLengh;            // COS的数据总长度
    unsigned char DataPackType;      // 数据包类型
    unsigned short DataPackSignal;  // 包号
    unsigned char cszAPPData[513];  // APP数据
    unsigned short CRC;     // CRC值
    unsigned int APPDataLenPer;    // 每包数据长度
} APPPACKDATA,*PAPPPACKDATA;
#pragma pack(pop)

//产品软硬件信息
typedef struct Struct_PRODUCTINFO{
	char CMSVer[32];
	char APPVer[32];
	char COSVer[32];
	char BuildVer[32];
	char SKFVer[32];//SKF版本号，, 以'\0'为结束符的ASCII字符串
	//char CustomName[64];// 客户名称, 以'\0'为结束符的ASCII字符串
	char DataTime[32];			//日期时间, 以'\0'为结束符的ASCII字符串
}PRODUCTINFO, *PPRODUCTINFO;


// 防控权限类型宏定义
#define SECURE_NEVER_ACCOUNT	0x00000000		//不允许
#define	 SECURE_ADM_ACCOUNT		0x00000001		//管理员权限
#define SECURE_USER_ACCOUNT		0x00000010		//用户权限
#define SECURE_ANYONE_ACCOUNT	0x000000FF		//任何人

// 设备状态
#define DEV_ABSENT_STATE     0x00000000			//设备不存在
#define DEV_PRESENT_STATE	0x00000001			//设备存在
#define DEV_UNKNOW_STATE	0x00000002			//设备状态未知

// PIN用户类型
#define ADMIN_TYPE	0
#define USER_TYPE   1

//算法标志
// SM1
#define SGD_SM1_ECB		0x00000101
#define SGD_SM1_CBC		0x00000102
#define SGD_SM1_CFB		0x00000104
#define SGD_SM1_OFB		0x00000108
#define SGD_SM1_MAC		0x00000110
// SM4
#define SGD_SMS4_ECB	0x00000401
#define SGD_SMS4_CBC	0x00000402			
#define SGD_SMS4_CFB	0x00000404			
#define SGD_SMS4_OFB	0x00000408			
// AES
#define SGD_AES128_ECB		0x00000801
#define SGD_AES128_CBC		0x00000802
#define SGD_AES128_CFB		0x00000804
#define SGD_AES128_OFB		0x00000808

#define SGD_AES192_ECB		0x00000811
#define SGD_AES192_CBC		0x00000812
#define SGD_AES192_CFB		0x00000814
#define SGD_AES192_OFB		0x00000818

#define SGD_AES256_ECB		0x00000821
#define SGD_AES256_CBC		0x00000822
#define SGD_AES256_CFB		0x00000824
#define SGD_AES256_OFB		0x00000828
// DES
#define SGD_DES_ECB		0x00001001
#define SGD_DES_CBC		0x00001002
#define SGD_DES_CFB		0x00001004
#define SGD_DES_OFB		0x00001008
// 3DES_2KEY
#define SGD_D3DES_ECB	0x00001011
#define SGD_D3DES_CBC	0x00001012
#define SGD_D3DES_CFB	0x00001014
#define SGD_D3DES_OFB	0x00001018
// 3DES_3KEY
#define SGD_T3DES_ECB	0x00001021
#define SGD_T3DES_CBC	0x00001022
#define SGD_T3DES_CFB	0x00001024
#define SGD_T3DES_OFB	0x00001028

//非对称
#define SGD_RSA		0x00010000
#define SGD_SM2_1	0x00020100			// 椭圆曲线签名算法
#define SGD_SM2_2	0x00020200			// 椭圆曲线密钥交换协议
#define SGD_SM2_3	0x00020400			// 椭圆曲线加密算法

//杂凑算法标志
#define SGD_SM3		0x00000001
#define SGD_SHA1	0x00000002
#define SGD_SHA256	0x00000004


#define SKF_USE_ENCDEC	0x01  //用于加密解密
#define SKF_USE_SIGVER	0x02  //用于签名验证

#define SYSTEM_RESULT_FILE "./systemresult.txt"
#define MOUNT_DIR "/media/wxtstorage"


/*
*设备管理
*/
unsigned int DEVAPI SKF_WaitForDevEvent(char * szDevName, unsigned int *pulDevNameLen, unsigned int *pulEvent);

unsigned int DEVAPI SKF_CancelWaitForDevEvent();

unsigned int DEVAPI SKF_EnumDev(unsigned int bPresent, char * szNameList, unsigned int *pulSize);

unsigned int DEVAPI SKF_ConnectDev (char * szName, DEVHANDLE *phDev);

unsigned int DEVAPI SKF_DisConnectDev (DEVHANDLE hDev);

unsigned int DEVAPI SKF_GetDevState(char * szDevName, unsigned int *pulDevState);

unsigned int DEVAPI SKF_SetLabel (DEVHANDLE hDev, char * szLabel);
// 功能有所扩展
unsigned int DEVAPI SKF_GetDevInfo (DEVHANDLE hDev, DEVINFO *pDevInfo);

unsigned int DEVAPI SKF_LockDev (DEVHANDLE hDev, unsigned int ulTimeOut);

unsigned int DEVAPI SKF_UnlockDev (DEVHANDLE hDev);

unsigned int DEVAPI SKF_Transmit(DEVHANDLE hDev, unsigned char *pbCommand, unsigned int ulCommandLen,unsigned char *pbData, unsigned int *pulDataLen);


/*
*访问控制
*/
unsigned int DEVAPI SKF_ChangeDevAuthKey (DEVHANDLE hDev, unsigned char *pbKeyValue, unsigned int ulKeyLen);

unsigned int DEVAPI SKF_DevAuth (DEVHANDLE hDev, unsigned char *pbAuthData, unsigned int ulLen);

unsigned int DEVAPI SKF_ChangePIN (HAPPLICATION hApplication, unsigned int ulPINType, char * szOldPin, char * szNewPin, unsigned int *pulRetryCount);

unsigned int DEVAPI SKF_GetPINInfo(HAPPLICATION hApplication, unsigned int ulPINType, unsigned int *pulMaxRetryCount, unsigned int *pulRemainRetryCount, unsigned int *pbDefaultPin);

unsigned int DEVAPI SKF_VerifyPIN (HAPPLICATION hApplication, unsigned int ulPINType, char * szPIN, unsigned int *pulRetryCount);

unsigned int DEVAPI SKF_UnblockPIN (HAPPLICATION hApplication, char * szAdminPIN, char * szNewUserPIN, unsigned int *pulRetryCount);

unsigned int DEVAPI SKF_ClearSecureState (HAPPLICATION hApplication);

/*
*应用管理
*/
unsigned int DEVAPI SKF_CreateApplication(DEVHANDLE hDev, char * szAppName, char * szAdminPin, unsigned int dwAdminPinRetryCount,char * szUserPin, unsigned int dwUserPinRetryCount,unsigned int dwCreateFileRights, HAPPLICATION *phApplication);

unsigned int DEVAPI SKF_EnumApplication(DEVHANDLE hDev, char * szAppName,unsigned int *pulSize);

unsigned int DEVAPI SKF_DeleteApplication(DEVHANDLE hDev, char * szAppName);

unsigned int DEVAPI SKF_OpenApplication(DEVHANDLE hDev, char * szAppName, HAPPLICATION *phApplication);

unsigned int DEVAPI SKF_CloseApplication(HAPPLICATION hApplication);


/*
*文件管理
*/
unsigned int DEVAPI SKF_CreateFile (HAPPLICATION hApplication, char * szFileName, unsigned int ulFileSize, unsigned int ulReadRights, unsigned int ulWriteRights);

unsigned int DEVAPI SKF_DeleteFile (HAPPLICATION hApplication, char * szFileName);

unsigned int DEVAPI SKF_EnumFiles (HAPPLICATION hApplication, char * szFileList, unsigned int *pulSize);

unsigned int DEVAPI SKF_GetFileInfo (HAPPLICATION hApplication, char * szFileName, FILEATTRIBUTE *pFileInfo);

unsigned int DEVAPI SKF_ReadFile (HAPPLICATION hApplication, char * szFileName, unsigned int ulOffset, unsigned int ulSize, unsigned char * pbOutData, unsigned int *pulOutLen);

unsigned int DEVAPI SKF_WriteFile (HAPPLICATION hApplication, char * szFileName, unsigned int ulOffset, unsigned char *pbData, unsigned int ulSize);

/*
*容器管理
*/
unsigned int DEVAPI SKF_CreateContainer (HAPPLICATION hApplication, char * szContainerName, HCONTAINER *phContainer);

unsigned int DEVAPI SKF_DeleteContainer(HAPPLICATION hApplication, char * szContainerName);

unsigned int DEVAPI SKF_OpenContainer(HAPPLICATION hApplication,char * szContainerName,HCONTAINER *phContainer);

unsigned int DEVAPI SKF_CloseContainer(HCONTAINER hContainer);

unsigned int DEVAPI SKF_EnumContainer (HAPPLICATION hApplication,char * szContainerName,unsigned int *pulSize);

unsigned int DEVAPI SKF_GetContainerType(HCONTAINER hContainer, unsigned int *pulContainerType);

unsigned int DEVAPI SKF_ImportCertificate(HCONTAINER hContainer, unsigned int bSignFlag, unsigned char* pbCert, unsigned int ulCertLen);

unsigned int DEVAPI SKF_ExportCertificate(HCONTAINER hContainer, unsigned int bSignFlag, unsigned char* pbCert, unsigned int *pulCertLen);

/*
*密码服务
*/
unsigned int DEVAPI SKF_GenRandom (DEVHANDLE hDev, unsigned char *pbRandom,unsigned int ulRandomLen);

unsigned int DEVAPI SKF_GenExtRSAKey (DEVHANDLE hDev, unsigned int ulBitsLen, RSAPRIVATEKEYBLOB *pBlob);

unsigned int DEVAPI SKF_GenRSAKeyPair (HCONTAINER hContainer, unsigned int ulBitsLen, RSAPUBLICKEYBLOB *pBlob);

unsigned int DEVAPI SKF_ImportRSAKeyPair (HCONTAINER hContainer, unsigned int ulSymAlgId, unsigned char *pbWrappedKey, unsigned int ulWrappedKeyLen, unsigned char *pbEncryptedData, unsigned int ulEncryptedDataLen);

unsigned int DEVAPI SKF_RSASignData(HCONTAINER hContainer, unsigned char *pbData, unsigned int ulDataLen, unsigned char *pbSignature, unsigned int *pulSignLen);

unsigned int DEVAPI SKF_RSAVerify (DEVHANDLE hDev , RSAPUBLICKEYBLOB *pRSAPubKeyBlob, unsigned char *pbData, unsigned int ulDataLen, unsigned char *pbSignature, unsigned int ulSignLen);

unsigned int DEVAPI SKF_RSAExportSessionKey (HCONTAINER hContainer, unsigned int ulAlgId, RSAPUBLICKEYBLOB *pPubKey, unsigned char *pbData, unsigned int *pulDataLen, HANDLE *phSessionKey);

unsigned int DEVAPI SKF_ExtRSAPubKeyOperation (DEVHANDLE hDev, RSAPUBLICKEYBLOB* pRSAPubKeyBlob,unsigned char* pbInput, unsigned int ulInputLen, unsigned char* pbOutput, unsigned int* pulOutputLen);

unsigned int DEVAPI SKF_ExtRSAPriKeyOperation (DEVHANDLE hDev, RSAPRIVATEKEYBLOB* pRSAPriKeyBlob,unsigned char* pbInput, unsigned int ulInputLen, unsigned char* pbOutput, unsigned int* pulOutputLen);

unsigned int DEVAPI SKF_GenECCKeyPair (HCONTAINER hContainer, unsigned int ulAlgId, ECCPUBLICKEYBLOB *pBlob);

unsigned int DEVAPI SKF_ImportECCKeyPair (HCONTAINER hContainer, PENVELOPEDKEYBLOB pEnvelopedKeyBlob);

unsigned int DEVAPI SKF_ECCSignData (HCONTAINER hContainer, unsigned char *pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

unsigned int DEVAPI SKF_ECCVerify (DEVHANDLE hDev , ECCPUBLICKEYBLOB* pECCPubKeyBlob, unsigned char *pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

unsigned int DEVAPI SKF_ECCExportSessionKey (HCONTAINER hContainer, unsigned int ulAlgId, ECCPUBLICKEYBLOB *pPubKey, PECCCIPHERBLOB pData, HANDLE *phSessionKey);

unsigned int DEVAPI SKF_ExtECCEncrypt (DEVHANDLE hDev, ECCPUBLICKEYBLOB* pECCPubKeyBlob,unsigned char* pbPlainText, unsigned int ulPlainTextLen, PECCCIPHERBLOB pCipherText);

unsigned int DEVAPI SKF_ExtECCDecrypt (DEVHANDLE hDev, ECCPRIVATEKEYBLOB* pECCPriKeyBlob, PECCCIPHERBLOB pCipherText, unsigned char* pbPlainText, unsigned int* pulPlainTextLen);

unsigned int DEVAPI SKF_ExtECCSign (DEVHANDLE hDev, ECCPRIVATEKEYBLOB* pECCPriKeyBlob, unsigned char* pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

// 功能有所扩展
unsigned int DEVAPI SKF_ExtECCVerify (DEVHANDLE hDev, ECCPUBLICKEYBLOB* pECCPubKeyBlob,unsigned char* pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

#if 0
//dwq 20181020
unsigned int DEVAPI SKF_ExtECCEncryptSuper (DEVHANDLE hDev, ECCPUBLICKEYBLOB* pECCPubKeyBlob,unsigned char* pbPlainText, unsigned int ulPlainTextLen, PECCCIPHERBLOB pCipherText);

unsigned int DEVAPI SKF_ExtECCDecryptSuper (DEVHANDLE hDev, ECCPRIVATEKEYBLOB* pECCPriKeyBlob, PECCCIPHERBLOB pCipherText, unsigned char* pbPlainText, unsigned int* pulPlainTextLen);

unsigned int DEVAPI SKF_ExtECCSignSuper (DEVHANDLE hDev, ECCPRIVATEKEYBLOB* pECCPriKeyBlob, unsigned char* pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);

unsigned int DEVAPI SKF_ExtECCVerifySuper (DEVHANDLE hDev, ECCPUBLICKEYBLOB* pECCPubKeyBlob,unsigned char* pbData, unsigned int ulDataLen, PECCSIGNATUREBLOB pSignature);
//
#endif

unsigned int DEVAPI SKF_GenerateAgreementDataWithECC (HCONTAINER hContainer, unsigned int ulAlgId,ECCPUBLICKEYBLOB* pTempECCPubKeyBlob,unsigned char* pbID, unsigned int ulIDLen,HANDLE *phAgreementHandle);

unsigned int DEVAPI SKF_GenerateAgreementDataAndKeyWithECC(HANDLE hContainer, unsigned int ulAlgId, ECCPUBLICKEYBLOB* pSponsorECCPubKeyBlob, ECCPUBLICKEYBLOB* pSponsorTempECCPubKeyBlob,
	ECCPUBLICKEYBLOB* pTempECCPubKeyBlob, unsigned char* pbID, unsigned int ulIDLen, unsigned char *pbSponsorID, unsigned int ulSponsorIDLen, HANDLE *phKeyHandle);

unsigned int DEVAPI SKF_GenerateKeyWithECC (HANDLE hAgreementHandle, ECCPUBLICKEYBLOB* pECCPubKeyBlob, ECCPUBLICKEYBLOB* pTempECCPubKeyBlob,
									 unsigned char* pbID, unsigned int ulIDLen, HANDLE *phKeyHandle);

unsigned int DEVAPI SKF_ExportPublicKey (HCONTAINER hContainer, unsigned int bSignFlag, unsigned char* pbBlob, unsigned int* pulBlobLen);

unsigned int DEVAPI SKF_ImportSessionKey (HCONTAINER hContainer, unsigned int ulAlgId, unsigned char *pbWrapedData,unsigned int ulWrapedLen ,HANDLE *phKey);

unsigned int DEVAPI SKF_SetSymmKey (DEVHANDLE hDev, unsigned char *pbKey, unsigned int ulAlgID, HANDLE *phKey);

unsigned int DEVAPI SKF_EncryptInit (HANDLE hKey, BLOCKCIPHERPARAM EncryptParam);

unsigned int DEVAPI SKF_Encrypt(HANDLE hKey, unsigned char * pbData, unsigned int ulDataLen, unsigned char *pbEncryptedData, unsigned int *pulEncryptedLen);

unsigned int DEVAPI SKF_EncryptUpdate(HANDLE hKey, unsigned char * pbData, unsigned int ulDataLen, unsigned char *pbEncryptedData, unsigned int *pulEncryptedLen);

unsigned int DEVAPI SKF_EncryptFinal (HANDLE hKey, unsigned char *pbEncryptedData, unsigned int *pulEncryptedDataLen );

unsigned int DEVAPI SKF_DecryptInit (HANDLE hKey, BLOCKCIPHERPARAM DecryptParam);

unsigned int DEVAPI SKF_Decrypt(HANDLE hKey, unsigned char * pbEncryptedData, unsigned int ulEncryptedLen, unsigned char * pbData, unsigned int * pulDataLen);

unsigned int DEVAPI SKF_DecryptUpdate(HANDLE hKey, unsigned char * pbEncryptedData, unsigned int ulEncryptedLen, unsigned char * pbData, unsigned int * pulDataLen);

unsigned int DEVAPI SKF_DecryptFinal (HANDLE hKey, unsigned char *pbDecryptedData, unsigned int *pulDecryptedDataLen);

unsigned int DEVAPI SKF_DigestInit(DEVHANDLE hDev, unsigned int ulAlgID, ECCPUBLICKEYBLOB *pPubKey, unsigned char *pucID, unsigned int ulIDLen, HANDLE *phHash);

unsigned int DEVAPI SKF_Digest (HANDLE hHash, unsigned char *pbData, unsigned int ulDataLen, unsigned char *pbHashData, unsigned int *pulHashLen);

unsigned int DEVAPI SKF_DigestUpdate (HANDLE hHash, unsigned char *pbData, unsigned int ulDataLen);

unsigned int DEVAPI SKF_DigestFinal (HANDLE hHash, unsigned char *pHashData, unsigned int *pulHashLen);

unsigned int DEVAPI SKF_MacInit (HANDLE hKey, BLOCKCIPHERPARAM *pMacParam, HANDLE *phMac);

unsigned int DEVAPI SKF_Mac(HANDLE hMac, unsigned char* pbData, unsigned int ulDataLen, unsigned char *pbMacData, unsigned int *pulMacLen);

unsigned int DEVAPI SKF_MacUpdate(HANDLE hMac, unsigned char * pbData, unsigned int ulDataLen);

unsigned int DEVAPI SKF_MacFinal (HANDLE hMac, unsigned char *pbMacData, unsigned int *pulMacDataLen);

unsigned int DEVAPI SKF_CloseHandle(HANDLE hHandle);

// 非标准扩展接口
// 扩展接口
unsigned int DEVAPI SKFE_SetSN( DEVHANDLE hDev, char* SN, unsigned int SNLen);
unsigned int DEVAPI SKFE_GenExtECCKey(DEVHANDLE hDev, PECCPRIVATEKEYBLOB pPriBlob, PECCPUBLICKEYBLOB pPubBlob);
// 功能有所扩展
unsigned int DEVAPI SKF_ECCDecrypt(HCONTAINER hContainer, PECCCIPHERBLOB pCipherText, unsigned char *pbPlainText,unsigned int *pulPlainTextLen);
unsigned int DEVAPI SKFE_ECCDecryptSignKey(HCONTAINER hContainer, PECCCIPHERBLOB pCipherText, unsigned char *pbPlainText,unsigned int *pulPlainTextLen);
// 扩展接口
unsigned int DEVAPI SKF_ImportPlainECCKeyPair(HCONTAINER hContainer, PECCPRIVATEKEYBLOB pPriBlob, PECCPUBLICKEYBLOB pPubBlob, unsigned int pKeyType );
unsigned int DEVAPI SKFE_GetFirmwareVer(DEVHANDLE hDev,unsigned char *pbFirmwareVer,unsigned int *ulFirmwareVerLen);

unsigned int DEVAPI SKFE_GetProductInfo(HANDLE hDev, PPRODUCTINFO pProductInfo);

//扩展接口，获取硬件平台类型，wangq, 2019.07.11
ULONG DEVAPI SKFE_GetPlatformType(HANDLE hDev, ULONG *pulPlatformType);

// 信工所添加的函数
// ECC数字信封转换并输出数字信封
unsigned int DEVAPI SKFE_ECCDigitalEnvelopTransform(DEVHANDLE hDev,HCONTAINER hContainer,PECCCIPHERBLOB pBeforeECCCipherBlob,PECCPUBLICKEYBLOB pPubBlob,PECCCIPHERBLOB pAfterECCCipherBlob);

#ifndef ANDROID
unsigned int DEVAPI SKFE_UpdateFirmware(char * szDevPath, unsigned char *FirmwareData, unsigned int FirmwareSize);
#endif
unsigned int DEVAPI SKFE_GetHwCode(DEVHANDLE hDev, unsigned char *pHwCode, unsigned int *pulHwCodeLen);

unsigned int DEVAPI SKFE_GetFwPublicKey(DEVHANDLE hDev, unsigned char *pucPublicKey, unsigned int *pulLen);

unsigned int DEVAPI SKFE_IssueCard();

// 河北CA添加的函数
#if 0
unsigned int DEVAPI SKF_GenerateKey(HCONTAINER hContainer, unsigned int ulAlgId, HANDLE *phSessionKey) ;
unsigned int DEVAPI SKF_ECCExportSessionKeyByHandle (HANDLE phSessionKey, ECCPUBLICKEYBLOB *pPubKey,PECCCIPHERBLOB pData);
unsigned int DEVAPI SKF_RSAExportSessionKeyByHandle (HANDLE phSessionKey, RSAPUBLICKEYBLOB*pPubKey,unsigned char *pbData, unsigned int *pulDataLen);
// unsigned int DEVAPI SKF_PrvKeyDecrypt(HCONTAINER hContainer,  PECCCIPHERBLOB pCipherText, unsigned char *pbData, unsigned int *pbDataLen  );
unsigned int DEVAPI SKF_PrvKeyDecrypt(HCONTAINER hContainer, unsigned int ulType, PECCCIPHERBLOB pCipherText, unsigned char *pbData, unsigned int *pbDataLen  );
unsigned int DEVAPI SKF_RSAPrvKeyDecrypt(HCONTAINER hContainer, unsigned char *pCipherData, unsigned int pCipherDataLen, unsigned char *pbData, unsigned int *pbDataLen );

#endif



/*-------- 错误值 -----------*/
#define SAR_OK 				0X00000000										//成功
#define SAR_FAIL 			0X0A000001										//失败
#define SAR_UNKNOWNERR 			0X0A000002									//异常错误
#define SAR_NOTSUPPORTYETERR 		0X0A000003								//不支持的服务
#define SAR_FILEERR 			0X0A000004									//文件操作错误
#define SAR_INVALIDHANDLEERR 		0X0A000005								//无效的句柄
#define SAR_INVALIDPARAMERR 		0X0A000006								//无效的参数
#define SAR_READFILEERR			0X0A000007									//读文件错误
#define SAR_WRITEFILEERR 		0X0A000008									//写文件错误
#define SAR_NAMELENERR			0X0A000009									//名称长度错误
#define SAR_KEYUSAGEERR			0X0A00000A									//密钥用途错误
#define SAR_MODULUSLENERR		0X0A00000B									//模的长度错误
#define SAR_NOTINITIALIZEERR		0X0A00000C								//未初始化
#define SAR_OBJERR			0X0A00000D										//对象错误
#define SAR_MEMORYERR			0X0A00000E									//内存错误
#define SAR_TIMEOUTERR			0X0A00000F									//超时
#define SAR_INDATALENERR		0X0A000010									//输入数据长度错误
#define SAR_INDATAERR			0X0A000011									//输入数据错误
#define SAR_GENRANDERR			0X0A000012									//生成随机数错误
#define SAR_HASHOBJERR			0X0A000013									//HASH对象错
#define SAR_HASHERR			0X0A000014										//HASH运算错误
#define SAR_GENRSAKEYERR		0X0A000015									//产生RSA密钥错
#define SAR_RSAMODULUSLENERR		0X0A000016								//RSA密钥模长错误
#define SAR_CSPIMPRTPUBKEYERR		0X0A000017								//CSP服务导入公钥错误
#define SAR_RSAENCERR			0X0A000018									//RSA加密错误
#define SAR_RSADECERR			0X0A000019									//RSA解密错误
#define SAR_HASHNOTEQUALERR		0X0A00001A									//HASH值不相等
#define SAR_KEYNOTFOUNTERR		0X0A00001B									//密钥未发现
#define SAR_CERTNOTFOUNTERR		0X0A00001C									//证书未发现
#define SAR_NOTEXPORTERR		0X0A00001D									//对象未导出
#define SAR_DECRYPTPADERR		0X0A00001E									//解密时做补丁错误
#define SAR_MACLENERR			0X0A00001F									//MAC长度错误
#define SAR_BUFFER_TOO_SMALL		0X0A000020								//缓冲区不足
#define SAR_KEYINFOTYPEERR		0X0A000021									//密钥类型错误
#define SAR_NOT_EVENTERR		0X0A000022									//无事件错误
#define SAR_DEVICE_REMOVED		0X0A000023									//设备已移除
#define SAR_PIN_INCORRECT		0X0A000024									//PIN不正确
#define SAR_PIN_LOCKED			0X0A000025									//PIN被锁死
#define SAR_PIN_INVALID			0X0A000026									//PIN无效
#define SAR_PIN_LEN_RANGE		0X0A000027									//PIN长度错误
#define SAR_USER_ALREADY_LOGGED_IN	0X0A000028								//用户已经登录
#define SAR_USER_PIN_NOT_INITIALIZED	0X0A000029							//没有初始化用户口令
#define SAR_USER_TYPE_INVALID		0X0A00002A								//PIN类型错误
#define SAR_APPLICATION_NAME_INVALID	0X0A00002B							//应用名称无效
#define SAR_APPLICATION_EXISTS		0X0A00002C								//应用已经存在
#define SAR_USER_NOT_LOGGED_IN		0X0A00002D								//用户没有登录
#define SAR_APPLICATION_NOT_EXISTS	0X0A00002E								//应用不存在
#define SAR_FILE_ALREADY_EXIST		0X0A00002F								//文件已经存在
#define SAR_NO_ROOM			0X0A000030										//空间不足
#define SAR_FILE_NOT_EXIST 0x0A000031										//文件不存在
#define SAR_REACH_MAX_CONTAINER_COUNT 0x0A000032							//已达到最大可管理容器数
#define SAR_AUTH_BLOCKED	0x0A000033										//密钥已被锁住
#define SAR_CERTNOUSAGEERR	0x0A000034										//证书未被使用
#define SAR_INVALIDCONTAINERERR	0x0A000035									//无效容器
#define SAR_CONTAINER_NOT_EXISTS	0X0A000036								//容器不存在
#define SAR_CONTAINER_EXISTS	0X0A000037									//容器已存在
#define	SAR_CERTUSAGEERR	0x0A000038										//证书已经被使用
#define SAR_KEYNOUSAGEERR	0x0A000039										//密钥未被使用
#define SAR_FILEATTRIBUTEERR	0x0A00003A									//文件操作权限错误
#define SAR_DEVNOAUTH			0x0A00003B									//设备未认证
#define SAR_PLEASE_INSERT_SD_AGAIN 0x0A00003C								//请重新插拔卡
#define SAR_APP_IS_NOT_ACTIVE   0x0A00003D						            //应用处于非活跃状态
#define SAR_APP_IS_OPENED       0x0A00003E                                  //应用处于打开状态

//固件自更新接口新增错误码
#define SAR_FW_DATA_FORMAT_INVALID					0x0A000040					//传入的固件数据无效(格式错误)
#define SAR_FW_SIGNATURE_INVALID							0x0A000041					//传入的固件数据的签名值无效(验签失败)



#ifdef __cplusplus
 }	
#endif

#endif	//__SKF_H__
