#ifndef __TEST_SERVER_DEFS_H__
#define __TEST_SERVER_DEFS_H__
#include "ServiceTypes.h"

#define TEST_SERVER_PORT		5001
#define MAX_PACKAGE_SIZE		1040

enum eMsgId
{
	MsgIdEcho = 0,
	MsgIdUpload = 1,
	MsgIdDownload = 2,
	MsgIdTest = 3
};

enum eFileMsgId
{
	FMsgIdName = 0,
	FMsgIdData = 1,
	FMsgIdOver = 2
};

#pragma pack(push, 1)

struct TestMsgBase
{
	CSUINT8		nMsgId;
};

struct EchoMsg : public TestMsgBase
{
	CSUINT16	nMsgSize;
	CSCHAR		szMsgBuf[1];
};

struct TransferFileMsg : public TestMsgBase
{
	CSUINT8		nFileMsgId;
	CSUINT32	nBufSize;
	CSBYTE		szData[1];
};

typedef EchoMsg	TestMsg;

#pragma pack(pop)

#endif // __TEST_SERVER_DEFS_H__