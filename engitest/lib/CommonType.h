#ifndef WXT_TYPE_H
#define WXT_TYPE_H

#include <stdio.h>
#include <stdlib.h>


#define MAX_PATH            260

#ifndef TRUE
#define TRUE	0x00000001
#endif
#ifndef FALSE
#define FALSE	0x00000000
#endif


#ifdef WIN32
#include <Windows.h>
#define DEVAPI __stdcall
#define WINAPI __stdcall
#else
#define __stdcall
#define _stdcall
#define DEVAPI __attribute__ ((visibility ("default")))
#define WINAPI __attribute__ ((visibility ("default")))
#endif

//the unit is millisecond in windows and microsecond in linux
#ifndef WIN32
#define Sleep(t) usleep(t*1000)
#endif


#define __declspec(x)  
#define __cdecl 
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

#define IN
#define OUT

typedef void* HANDLE;
typedef HANDLE DEVHANDLE;
typedef HANDLE HAPPLICATION;
typedef HANDLE HCONTAINER;


typedef char			INT8;
typedef short			INT16;
typedef int				INT;
typedef int				INT32;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef int				BOOL;
typedef UINT8			BYTE;
typedef INT8			CHAR;
typedef char *          PCHAR;
typedef UINT8			UCHAR;
typedef INT16			SHORT;
typedef UINT16			USHORT;
typedef INT32			LONG;
typedef UINT32			ULONG;
typedef UINT32			UINT;
typedef UINT16			WORD;
typedef UINT32			DWORD;
typedef	UINT32			FLAGES;
typedef CHAR*			LPSTR;
typedef void*           PVOID;
typedef void*           LPVOID;
typedef DWORD*          LPDWORD;


/*-------- ����ֵ -----------*/
#define SAR_OK 				0X00000000										//�ɹ�
#define SAR_FAIL 			0X0A000001										//ʧ��
#define SAR_UNKNOWNERR 			0X0A000002									//�쳣����
#define SAR_NOTSUPPORTYETERR 		0X0A000003								//��֧�ֵķ���
#define SAR_FILEERR 			0X0A000004									//�ļ���������
#define SAR_INVALIDHANDLEERR 		0X0A000005								//��Ч�ľ��
#define SAR_INVALIDPARAMERR 		0X0A000006								//��Ч�Ĳ���
#define SAR_READFILEERR			0X0A000007									//���ļ�����
#define SAR_WRITEFILEERR 		0X0A000008									//д�ļ�����
#define SAR_NAMELENERR			0X0A000009									//���Ƴ��ȴ���
#define SAR_KEYUSAGEERR			0X0A00000A									//��Կ��;����
#define SAR_MODULUSLENERR		0X0A00000B									//ģ�ĳ��ȴ���
#define SAR_NOTINITIALIZEERR		0X0A00000C								//δ��ʼ��
#define SAR_OBJERR			0X0A00000D										//�������
#define SAR_MEMORYERR			0X0A00000E									//�ڴ����
#define SAR_TIMEOUTERR			0X0A00000F									//��ʱ
#define SAR_INDATALENERR		0X0A000010									//�������ݳ��ȴ���
#define SAR_INDATAERR			0X0A000011									//�������ݴ���
#define SAR_GENRANDERR			0X0A000012									//�������������
#define SAR_HASHOBJERR			0X0A000013									//HASH�����
#define SAR_HASHERR			0X0A000014										//HASH�������
#define SAR_GENRSAKEYERR		0X0A000015									//����RSA��Կ��
#define SAR_RSAMODULUSLENERR		0X0A000016								//RSA��Կģ������
#define SAR_CSPIMPRTPUBKEYERR		0X0A000017								//CSP�����빫Կ����
#define SAR_RSAENCERR			0X0A000018									//RSA���ܴ���
#define SAR_RSADECERR			0X0A000019									//RSA���ܴ���
#define SAR_HASHNOTEQUALERR		0X0A00001A									//HASHֵ�����
#define SAR_KEYNOTFOUNTERR		0X0A00001B									//��Կδ����
#define SAR_CERTNOTFOUNTERR		0X0A00001C									//֤��δ����
#define SAR_NOTEXPORTERR		0X0A00001D									//����δ����
#define SAR_DECRYPTPADERR		0X0A00001E									//����ʱ����������
#define SAR_MACLENERR			0X0A00001F									//MAC���ȴ���
#define SAR_BUFFER_TOO_SMALL		0X0A000020								//����������
#define SAR_KEYINFOTYPEERR		0X0A000021									//��Կ���ʹ���
#define SAR_NOT_EVENTERR		0X0A000022									//���¼�����
#define SAR_DEVICE_REMOVED		0X0A000023									//�豸���Ƴ�
#define SAR_PIN_INCORRECT		0X0A000024									//PIN����ȷ
#define SAR_PIN_LOCKED			0X0A000025									//PIN������
#define SAR_PIN_INVALID			0X0A000026									//PIN��Ч
#define SAR_PIN_LEN_RANGE		0X0A000027									//PIN���ȴ���
#define SAR_USER_ALREADY_LOGGED_IN	0X0A000028								//�û��Ѿ���¼
#define SAR_USER_PIN_NOT_INITIALIZED	0X0A000029							//û�г�ʼ���û�����
#define SAR_USER_TYPE_INVALID		0X0A00002A								//PIN���ʹ���
#define SAR_APPLICATION_NAME_INVALID	0X0A00002B							//Ӧ��������Ч
#define SAR_APPLICATION_EXISTS		0X0A00002C								//Ӧ���Ѿ�����
#define SAR_USER_NOT_LOGGED_IN		0X0A00002D								//�û�û�е�¼
#define SAR_APPLICATION_NOT_EXISTS	0X0A00002E								//Ӧ�ò�����
#define SAR_FILE_ALREADY_EXIST		0X0A00002F								//�ļ��Ѿ�����
#define SAR_NO_ROOM			0X0A000030										//�ռ䲻��
#define SAR_FILE_NOT_EXIST 0x0A000031										//�ļ�������
#define SAR_REACH_MAX_CONTAINER_COUNT 0x0A000032							//�Ѵﵽ���ɹ���������
#define SAR_AUTH_BLOCKED	0x0A000033										//��Կ�ѱ���ס
#define SAR_CERTNOUSAGEERR	0x0A000034										//֤��δ��ʹ��
#define SAR_INVALIDCONTAINERERR	0x0A000035									//��Ч����
#define SAR_CONTAINER_NOT_EXISTS	0X0A000036								//����������
#define SAR_CONTAINER_EXISTS	0X0A000037									//�����Ѵ���
#define	SAR_CERTUSAGEERR	0x0A000038										//֤���Ѿ���ʹ��
#define SAR_KEYNOUSAGEERR	0x0A000039										//��Կδ��ʹ��
#define SAR_FILEATTRIBUTEERR	0x0A00003A									//�ļ�����Ȩ�޴���
#define SAR_DEVNOAUTH			0x0A00003B									//�豸δ��֤
#define SAR_PLEASE_INSERT_SD_AGAIN 0x0A00003C								//�����²�ο�
#define SAR_APP_IS_NOT_ACTIVE   0x0A00003D						            //Ӧ�ô��ڷǻ�Ծ״̬
#define SAR_APP_IS_OPENED       0x0A00003E                                  //Ӧ�ô��ڴ�״̬

//�̼��Ը��½ӿ�����������
#define SAR_FW_DATA_FORMAT_INVALID					0x0A000040					//����Ĺ̼�������Ч(��ʽ����)
#define SAR_FW_SIGNATURE_INVALID							0x0A000041					//����Ĺ̼����ݵ�ǩ��ֵ��Ч(��ǩʧ��)

#endif
