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

#endif
