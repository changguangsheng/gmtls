
/* ECC �ӿ� */

#include <stdio.h>
#include "CommonType.h"
#include "skf.h"
#include "dev_ecc_funs.h"

// �����ṹ��˵��

/*==> ECCPUBLICKEYBLOB ECC��Կ�ṹ��Ա˵��
	typedef struct Struct_ECCPUBLICKEYBLOB{
		ULONG BitLen;				// ECC�㷨���ȱ�ʶ �̶�Ϊ256
		BYTE XCoordinate[64];		// XCoordinate�ĺ�32λ  ���ECC��Կ��ǰ32�ֽ�
		BYTE YCoordinate[64];		// YCoordinate�ĺ�32λ  ���ECC��Կ�ĺ�32�ֽ�	
	}ECCPUBLICKEYBLOB, *PECCPUBLICKEYBLOB;
<==*/

/*==> ECCPUBLICKEYBLOB ECCǩ�����ݽṹ˵��
	typedef struct Struct_ECCSIGNATUREBLOB{
		BYTE r[64];				// ����ĺ�32λ	���ǩ��ֵ��r
		BYTE s[64];				// ����ĺ�32λ	���ǩ��ֵ��s
	} ECCSIGNATUREBLOB, *PECCSIGNATUREBLOB;
<==*/

/*==>ECCCIPHERBLOB ECC�������ݽṹ˵��
	typedef struct Struct_ECCCIPHERBLOB{
		BYTE XCoordinate[64];		// ��y�����Բ�����ϵĵ�(x,y)
		BYTE YCoordinate[64];		// ��x�����Բ�����ϵĵ�(x,y)
		BYTE HASH[32];				// �����Ӵ�ֵ
		ULONG CipherLen;			// �������ݳ���
		BYTE Cipher[1];				// ��������
	} ECCCIPHERBLOB, *PECCCIPHERBLOB;
<==*/

/*==>ECCPRIVATEKEYBLOB ECC˽Կ�ṹ˵��
	typedef struct Struct_ECCPRIVATEKEYBLOB{
		ULONG BitLen;			// ECC�㷨���ȱ�ʶ �̶�Ϊ256
		BYTE PrivateKey[64];	// PrivateKey�ĺ�32λ  ���ECC��˽Կ
	}ECCPRIVATEKEYBLOB, *PECCPRIVATEKEYBLOB;
<==*/

/*==>ENVELOPEDKEYBLOB ECC������Կ�Ա����ṹ˵��
typedef struct SKF_ENVELOPEDKEYBLOB{
	ULONG Version;						 // ��ǰ�汾 �̶�Ϊ1
	ULONG ulSymmAlgID;					 // �Գ��㷨��ʶ���޶�ECBģʽ, ��SGD_SM1_ECB
	ULONG ulBits;						 // ������Կ�Ե���Կλ����, �̶�Ϊ256
	BYTE cbEncryptedPriKey[64];			 // ������Կ��˽Կ�����ģ��ûỰ��Կ����ECC˽Կ�Ľ��
	ECCPUBLICKEYBLOB PubKey;			 // ������Կ�ԵĹ�Կ
	ECCCIPHERBLOB ECCCipherBlob;		 // �ñ�����Կ(ǩ����Կ)���ܵĶԳ���Կ����  
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

	// ����ECCǩ����Կ��
	// ���豸����ECCǩ����Կ�� ��������Կ��pubBlob,SGD_SM2_1ΪECC�㷨��ʶ
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

	// ECCǩ��(ʹ��ECCǩ��˽Կǩ��)
	// bSignDataΪ��ǩ������,ulSignDataLenΪ��ǩ�����ݳ���,����С��32�ֽ�; tEccSignBlobΪǩ������ṹ
	rv = SKF_ECCSignData(hContainer, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCSignData Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ECCSignData OK\n");

	// ECC��ǩ
	// tEccPubBlobΪECCǩ����Կ,bSignDataΪ��ǩ������,ulSignDataLen��ǩ�����ݳ���,tEccSignBlobΪSKF_ECCSignData�õ���ǩ��ֵ
	rv = SKF_ECCVerify(hDev, &tEccPubBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ECCVerify Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ECCVerify OK\n");
	
	// ECC������Կ����
	// tExtEccPubkeyBlob������Կ, bIndataΪ����������, ulIndataLneΪ���������ݳ���, pEccCipherBlobΪ���ܽ��
	// ע������ECCCIPHERBLOB�������ݳ�ԱCipher[1] ����ֻ��һ���ֽ�, Ϊ�˱�֤���ݵ�����,pEccCipherBlob��Ҫͨ��malloc�����㹻��Ŀռ�
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
	

	// ����˽�н���
	// tExtEccPrikeyBlobΪ˽Կ�ṹ, pEccCipherBlobΪ���ܽ���ṹ, bEccDecDataΪ���ܽ��, ulEccDecDataLen���ܽ�����ݳ���
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
	// ECC������Կ���ܣ�super��
	// tExtEccPubkeyBlob������Կ, bIndataΪ����������, ulIndataLneΪ���������ݳ���, pEccCipherBlobΪ���ܽ��
	// ע������ECCCIPHERBLOB�������ݳ�ԱCipher[1] ����ֻ��һ���ֽ�, Ϊ�˱�֤���ݵ�����,pEccCipherBlob��Ҫͨ��malloc�����㹻��Ŀռ�
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
	

	// ����˽�н��ܣ�super��
	// tExtEccPrikeyBlobΪ˽Կ�ṹ, pEccCipherBlobΪ���ܽ���ṹ, bEccDecDataΪ���ܽ��, ulEccDecDataLen���ܽ�����ݳ���
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

	// ����ECC�ӽ�����Կ��
	//==> ����ģ�� Ӧ�÷���ȡ�Ự��Կ���ġ�˽Կ���Ĺ���
	// ʹ��ǩ����Կ���ܻỰ��Կ, �õ��Ự��Կ����
	memset(pEccCipherBlob, 0x00, sizeof(ECCCIPHERBLOB) + ulIndataLen);
	rv = SKF_ExtECCEncrypt(hDev, &tEccPubBlob, bIndata, ulIndataLen, pEccCipherBlob);
	// ʹ�ûỰ��Կ����˽Կ, �õ�ECC˽Կ����
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
	
	
	// ����˽Կǩ��
	rv = SKF_ExtECCSign(hDev, &tExtEccPrikeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCSign Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCSign OK\n");

	// ������Կ��ǩ
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
	// ����˽Կǩ��
	rv = SKF_ExtECCSignSuper(hDev, &tExtEccPrikeyBlob, bSignData, ulSignDataLen, &tEccSignBlob);
	if (rv != SAR_OK)
	{
		PrintMsg("SKF_ExtECCSignSuper Wrong\n");
		fflush(stdin);
		getchar();
		return ;
	}
	PrintMsg("SKF_ExtECCSignSuper OK\n");

	// ������Կ��ǩ
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
	// ������Կ
	// bSignFlag ΪTRUEʱ��ʾ����ǩ����Կ, FALSEʱ��ʾ�����ӽ��ܹ�Կ, pExportPubkeyָ��ECC/RSA��Կ�ṹ
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

	// ���ܹ�Կ�ԻỰ��Կ����, �õ��Ự��Կ����
	//==> ����ģ�� Ӧ�÷�ʹ��SKF_ImportECCKeyPair����Ĺ�˽Կ���еĹ�Կ�ԻỰ��Կ����
	memset(pEccCipherBlob, 0x00, sizeof(ECCCIPHERBLOB) + ulIndataLen);
	rv = SKF_ExtECCEncrypt(hDev, &tExtEccPubkeyBlob, bIndata, ulIndataLen, pEccCipherBlob);
	//<== end
	// SGD_SM1_ECB��ʶ�Ự��Կ�㷨��ʶ, pEccCipherBlobΪ�Ự��Կ����,sizeof(PECCCIPHERBLOB) + ulIndataLen�Ự��Կ���ĳ���
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

	// ECC���ɲ������Ự��Կ
	// ʹ��tExtEccPubkeyBlob���ܲ����ĻỰ��Կ, ���ؼ��ܽ������pEccCipherBlob�ͻỰ��Կ���hKey
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