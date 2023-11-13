#pragma once

#include <process.h>
#include <thread>
#include <mutex>
#include <filesystem>
#include "AsFileUtils.h"

namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_LOADING, LEVEL_EDIT, 
		LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace Client;

//#ifdef _DEBUG
//
//#ifdef UNICODE                                                                                      
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
//#else                                                                                                    
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
//#endif                                                                                                   
//
//#endif  