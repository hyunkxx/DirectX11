#pragma once

#include "Client_Enum.h"
#include "Client_ShaderEnum.h"
#include "Client_TextureEnum.h"
#include "Client_ObjectEnum.h"
#include "Client_ModelEnum.h"

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;