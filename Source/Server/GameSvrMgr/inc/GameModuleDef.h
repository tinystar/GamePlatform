/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/26
/************************************************************************/
#ifndef __GAME_MODULE_DEF_H__
#define __GAME_MODULE_DEF_H__

class GameDeskFactory;

enum AppMsgCode
{
	kNullMsg = 0,
	kInitAppMsg,
	kUnloadAppMsg
};


typedef bool (*GameAppEntryPointFuncPtr)(AppMsgCode);
typedef GameDeskFactory* (*GetGameDeskFactoryFuncPtr)(void);
typedef const char* (*GetGameAppVersionFuncPtr)(void);

#define GAMEAPPENTRYPOINT_PROC_NAME		"GameAppEntryPoint"
#define GETGAMEDESKFACTORY_PROC_NAME	"GetGameDeskFactory"
#define GETGAMEAPPVERSION_PROC_NAME		"GetGameAppVersion"


// Change this version string if the SDK is updated.
// It's intent is to solve the DLL hell problem.
#define GAME_VERSION_STRING				"1.0.0"

#endif // __GAME_MODULE_DEF_H__
