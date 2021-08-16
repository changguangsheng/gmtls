
/* ECC 接口 */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_ecc_funs.h"

// 几个结构体说明

/*==> ECCPUBLICKEYBLOB ECC公钥结构成员说明
	typedef struct Struct_ECCPUBLICKEYBLOB{
		ULONG BitLen;				// ECC算法长度标识 固定为256
		BYTE XCoordinate[64];		// XCoordinate的后32位  存放ECC公钥的前32字节
		BYTE YCoordinate[64];		// YCoordinate的后32位  存放ECC公钥的后32字节	
	}ECCPUBLICKEYBLOB, *PECCPUBLICKEYBLOB;
<==*/

/*==> ECCPUBLICKEYBLOB ECC签名数据结构说明
	typedef struct Struct_ECCSIGNATUREBLOB{
		BYTE r[64];				// 数组的后32位	存放签名值的r
		BYTE s[64];				// 数组的后32位	存放签名值的s
	} ECCSIGNATUREBLOB, *PECCSIGNATUREBLOB;
<==*/

/*==>ECCCIPHERBLOB ECC密文数据结构说明
	typedef struct Struct_ECCCIPHERBLOB{
		BYTE XCoordinate[64];		// 与y组成椭圆曲线上的点(x,y)
		BYTE YCoordinate[64];		// 与x组成椭圆曲线上的点(x,y)
		BYTE HASH[32];				// 明文杂凑值
		ULONG CipherLen;			// 密文数据长度
		BYTE Cipher[1];				// 密文数据
	} ECCCIPHERBLOB, *PECCCIPHERBLOB;
<==*/

/*==>ECCPRIVATEKEYBLOB ECC私钥结构说明
	typedef struct Struct_ECCPRIVATEKEYBLOB{
		ULONG BitLen;			// ECC算法长度标识 固定为256
		BYTE PrivateKey[64];	// PrivateKey的后32位  存放ECC的私钥
	}ECCPRIVATEKEYBLOB, *PECCPRIVATEKEYBLOB;
<==*/

/*==>ENVELOPEDKEYBLOB ECC加密密钥对保护结构说明
typedef struct SKF_ENVELOPEDKEYBLOB{
	ULONG Version;						 // 当前版本 固定为1
	ULONG ulSymmAlgID;					 // 对称算法标识，限定ECB模式, 如SGD_SM1_ECB
	ULONG ulBits;						 // 加密密钥对的密钥位长度, 固定为256
	BYTE cbEncryptedPriKey[64];			 // 加密密钥对私钥的密文，用会话密钥加密ECC私钥的结果
	ECCPUBLICKEYBLOB PubKey;			 // 加密密钥对的公钥
	ECCCIPHERBLOB ECCCipherBlob;		 // 用保护公钥(签名公钥)加密的对称密钥密文  
}ENVELOPEDKEYBLOB, *PENVELOPEDKEYBLOB;
<==*/

void dev_ecc_funs_test()
{
	ULONG rv = 0;
	ECCPUBLICKEYBLOB tEccPubBlob;
	ULONG ulContainerType = 0;
	BYTE bSignData[32] = {0};
	ULONG ulSignDataLen = 32;
	ECCSIGNATUREBLOB tEccSignBlob;

	ECCPUBLICKEYBLOB tExtEccPubkeyBlob;
	BYTE bIndata[16] = {0x37,0xB7,0xE3,0x25,0x9C,0x36,0xE3,0x8E,0xC3,0xA5,0x85,0x86,0x10,0xb5,0xb0,0x7a};
	ULONG ulIndataLen = 16;
	ECCCIPHERBLOB* pEccCipherBlob = NULL; 

	ECCPRIVATEKEYBLOB tExtEccPrikeyBlob;
	BYTE bEccPrikey[] = {		
		0xB1,0xE7,0xFD,0xCB,0x32,0x12,0x1C,0x67,0x3A,0xB7,0x99,0xE5,0xED,0x7B,0xD7,0x86,
		0x60,0xA3,0xA1,0x54,0x30,0x55,0xDB,0x4A,0x0D,0x94,0xD0,0xEF,0xB6,0x98,0x56,0x73
	};
	BYTE bEccPubkey[] = {	
		0xF0,0x80,0x36,0x1D,0x43,0xE6,0x5B,0x47,0xE8,0xF0,0xD2,0xC1,0x5E,0x99,0x98,0x5E,
		0xD7,0x86,0xED,0x29,0x30,0x8D,0xFF,0xAB,0xB5,0xF0,0x43,0x21,0x6A,0xD6,0x87,0xC2,
		0x50,0x73,0x3E,0x09,0xE0,0x1A,0x48,0xF3,0xBA,0xA5,0xCD,0x7E,0x90,0x35,0xFD,0x76,
		0x6C,0xEB,0x7B,0xFD,0x4D,0x23,0x48,0xA2,0x66,0x94,0x2D,0xBC,0x10,0xE4,0x84,0x56
	};
	BYTE bEccDecData[256] = {0};
	ULONG ulEccDecDataLen = 256;

	ENVELOPEDKEYBLOB* EnvdKeyBlob = NULL;
	HANDLE hKey = NULL;
	BLOCKCIPHERPARAM EncParam;
	BYTE EncData[64] = {0};
	ULONG EncDataLen = 64;

	BOOL bSignFlag = FALSE;
	BYTE* pExportPubkey = NULL;
	ULONG ulExportPubkeyLen = 64;

	BYTE bySymKeyPlain[64] = {0};
	ULONG ulSymKeyPlainLen = 64; 

	rv = SKF_OpenContainer(hApplication, "Test_Container", &hContainer);

	// 生成ECC签名密钥对
	// 由设备产生ECC签名密钥对 并导出公钥到pubBlob,SGD_SM2_1为ECC算法标识
	rv = SKF_GenECCKeyPair(hContainer, SGD_SM2_1, &tEccPubBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_GenECCKeyPair Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	rv = SKF_GetContainerType(hContainer, &ulContainerType);
	PrintMsg("SKF_GenECCKeyPair ContainerType[%u] OK\n", ulContainerType);

	// ECC签名(使用ECC签名私钥签名)
	// bSignData为待签名数据,ulSignDataLen为待签名数据长度,必须小于32字节; tEccSignBlob为签名结果结构
	rv = SKF_ECCSignData(hContainer, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCSignData Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ECCSignData OK\n");

	// ECC验签
	// tEccPubBlob为ECC签名公钥,bSignData为待签名数据,ulSignDataLen待签名数据长度,tEccSignBlob为SKF_ECCSignData得到的签名值
	rv = SKF_ECCVerify(hDev, &tEccPubBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCVerify Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ECCVerify OK\n");
	
	// ECC外来公钥加密
	// tExtEccPubkeyBlob外来公钥, bIndata为待加密数据, ulIndataLne为待加密数据长度, pEccCipherBlob为加密结果
	// 注：由于ECCCIPHERBLOB密文数据成员Cipher[1] 给定只有一个字节, 为了保证数据的完整,pEccCipherBlob需要通过malloc开辟足够大的空间
	pEccCipherBlob = (ECCCIPHERBLOB*)malloc(sizeof(ECCCIPHERBLOB)*2);
	if (pEccCipherBlob == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	tExtEccPubkeyBlob.BitLen = 256;
	memcpy(tExtEccPubkeyBlob.XCoordinate + 32, bEccPubkey, 32);
	memcpy(tExtEccPubkeyBlob.YCoordinate + 32, bEccPubkey + 32, 32);
	rv = SKF_ExtECCEncrypt(hDev, &tExtEccPubkeyBlob, bIndata, ulIndataLen, pEccCipherBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCEncrypt Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCEncrypt OK\n");
	

	// 外来私有解密
	// tExtEccPrikeyBlob为私钥结构, pEccCipherBlob为加密结果结构, bEccDecData为解密结果, ulEccDecDataLen解密结果数据长度
	tExtEccPrikeyBlob.BitLen = 256;
	memcpy(tExtEccPrikeyBlob.PrivateKey + 32, bEccPrikey, sizeof(bEccPrikey));
	rv = SKF_ExtECCDecrypt(hDev, &tExtEccPrikeyBlob, pEccCipherBlob, bEccDecData, &ulEccDecDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCDecrypt Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCDecrypt OK\n");
	
#if 0
	//dwq 20181020 start
	// ECC外来公钥加密（super）
	// tExtEccPubkeyBlob外来公钥, bIndata为待加密数据, ulIndataLne为待加密数据长度, pEccCipherBlob为加密结果
	// 注：由于ECCCIPHERBLOB密文数据成员Cipher[1] 给定只有一个字节, 为了保证数据的完整,pEccCipherBlob需要通过malloc开辟足够大的空间
	pEccCipherBlob = (ECCCIPHERBLOB*)malloc(sizeof(ECCCIPHERBLOB)*2);
	if (pEccCipherBlob == NULL)
	{
		PrintMsg("malloc wrong !");
		return ;
	}
	tExtEccPubkeyBlob.BitLen = 256;
	memcpy(tExtEccPubkeyBlob.XCoordinate + 32, bEccPubkey, 32);
	memcpy(tExtEccPubkeyBlob.YCoordinate + 32, bEccPubkey + 32, 32);
	rv = SKF_ExtECCEncryptSuper(hDev, &tExtEccPubkeyBlob, bIndata, ulIndataLen, pEccCipherBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCEncryptSuper Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCEncryptSuper OK\n");
	

	// 外来私有解密（super）
	// tExtEccPrikeyBlob为私钥结构, pEccCipherBlob为加密结果结构, bEccDecData为解密结果, ulEccDecDataLen解密结果数据长度
	tExtEccPrikeyBlob.BitLen = 256;
	memcpy(tExtEccPrikeyBlob.PrivateKey + 32, bEccPrikey, sizeof(bEccPrikey));
	rv = SKF_ExtECCDecryptSuper(hDev, &tExtEccPrikeyBlob, pEccCipherBlob, bEccDecData, &ulEccDecDataLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCDecryptSuper Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCDecryptSuper OK\n");
	//dwq 20181020 end
#endif

	// 导入ECC加解密密钥对
	//==> 以下模拟 应用方获取会话密钥密文、私钥密文过程
	// 使用签名公钥加密会话密钥, 得到会话密钥密文
	memset(pEccCipherBlob, 0x00, sizeof(ECCCIPHERBLOB) + ulIndataLen);
	rv = SKF_ExtECCEncrypt(hDev, &tEccPubBlob, bIndata, ulIndataLen, pEccCipherBlob);
	// 使用会话密钥加密私钥, 得到ECC私钥密文
	rv = SKF_SetSymmKey(hDev, bIndata, SGD_SM1_ECB, &hKey);
	EncParam.IVLen = 0;
	EncParam.PaddingType = 0;
	rv = SKF_EncryptInit(hKey, EncParam);
	rv = SKF_Encrypt(hKey, tExtEccPrikeyBlob.PrivateKey, 64, EncData, &EncDataLen);
	rv = SKF_CloseHandle(hKey);
	//<== end
	EnvdKeyBlob = (ENVELOPEDKEYBLOB*)malloc(sizeof(ENVELOPEDKEYBLOB) * 2);
	EnvdKeyBlob->ulBits = 256;
	EnvdKeyBlob->ulSymmAlgID = SGD_SM1_ECB;
	EnvdKeyBlob->PubKey = tExtEccPubkeyBlob;
	memcpy(&(EnvdKeyBlob->ECCCipherBlob), pEccCipherBlob, sizeof(ECCCIPHERBLOB) + pEccCipherBlob->CipherLen +1);
	memcpy(EnvdKeyBlob->cbEncryptedPriKey, EncData, EncDataLen);
	rv = SKF_ImportECCKeyPair(hContainer, EnvdKeyBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportECCKeyPair Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ImportECCKeyPair OK\n");

	rv = SKF_ImportPlainECCKeyPair(hContainer, &tExtEccPrikeyBlob,&tExtEccPubkeyBlob,SKF_USE_ENCDEC);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportPlainECCKeyPair SKF_USE_ENCDEC Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ImportPlainECCKeyPair SKF_USE_ENCDEC OK\n");

	rv = SKF_ImportPlainECCKeyPair(hContainer,&tExtEccPrikeyBlob,&tExtEccPubkeyBlob,SKF_USE_SIGVER);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportPlainECCKeyPair SKF_USE_SIGVER Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ImportPlainECCKeyPair SKF_USE_SIGVER OK\n");
	
	
	// 外来私钥签名
	rv = SKF_ExtECCSign(hDev, &tExtEccPrikeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCSign Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCSign OK\n");

	// 外来公钥验签
	rv = SKF_ExtECCVerify(hDev, &tExtEccPubkeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCVerify Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCVerify OK\n");

#if 0
	//dwq 20181020 start
	// 外来私钥签名
	rv = SKF_ExtECCSignSuper(hDev, &tExtEccPrikeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCSignSuper Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCSignSuper OK\n");

	// 外来公钥验签
	rv = SKF_ExtECCVerifySuper(hDev, &tExtEccPubkeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCVerifySuper Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCVerifySuper OK\n");
	//dwq 20181020 end
#endif
	// 导出公钥
	// bSignFlag 为TRUE时表示导出签名公钥, FALSE时表示导出加解密公钥, pExportPubkey指向ECC/RSA公钥结构
	pExportPubkey = (BYTE*)&tExtEccPubkeyBlob;
	ulExportPubkeyLen = sizeof(ECCPUBLICKEYBLOB)+1;
	rv = SKF_ExportPublicKey(hContainer, bSignFlag, pExportPubkey, &ulExportPubkeyLen);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExportPublicKey Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExportPublicKey OK\n");

	// 加密公钥对会话密钥加密, 得到会话密钥密文
	//==> 以下模拟 应用方使用SKF_ImportECCKeyPair导入的公私钥对中的公钥对会话密钥加密
	memset(pEccCipherBlob, 0x00, sizeof(ECCCIPHERBLOB) + ulIndataLen);
	rv = SKF_ExtECCEncrypt(hDev, &tExtEccPubkeyBlob, bIndata, ulIndataLen, pEccCipherBlob);
	//<== end
	// SGD_SM1_ECB标识会话密钥算法标识, pEccCipherBlob为会话密钥密文,sizeof(PECCCIPHERBLOB) + ulIndataLen会话密钥密文长度
	rv = SKF_ImportSessionKey (hContainer, SGD_SM1_ECB, (BYTE *)pEccCipherBlob, sizeof(PECCCIPHERBLOB) + ulIndataLen, &hKey);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ImportSessionKey Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ImportSessionKey OK\n");
	rv = SKF_CloseHandle(hKey);

	// ECC生成并导出会话密钥
	// 使用tExtEccPubkeyBlob加密产生的会话密钥, 返回加密结果密文pEccCipherBlob和会话密钥句柄hKey
	memset(pEccCipherBlob, 0x00, sizeof(ECCCIPHERBLOB) + ulIndataLen);
	rv = SKF_ECCExportSessionKey(hContainer, SGD_SM1_ECB, &tExtEccPubkeyBlob, pEccCipherBlob, &hKey);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCExportSessionKey Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ECCExportSessionKey OK\n");
	rv = SKF_CloseHandle(hKey);	
	
	rv = SKF_CloseContainer(hContainer);
	free(pEccCipherBlob);
	pEccCipherBlob = NULL;
}