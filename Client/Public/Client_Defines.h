#pragma once

#include "Client_Enum.h"
#include "Client_StaticTextureEnum.h"
#include "Client_ShaderEnum.h"
#include "Client_TextureEnum.h"
#include "Client_ObjectEnum.h"
#include "Client_ModelEnum.h"
#include "Client_InstanceModelEnum.h"

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

// AnimTool
extern int g_iGUIID;

#define VOLUME_BGM CGameInstance::GetInstance()->GetVolume(CSound_Manager::SOUND_TYPE::SOUND_BGM)
#define VOLUME_VFX  CGameInstance::GetInstance()->GetVolume(CSound_Manager::SOUND_TYPE::SOUND_VFX)
#define VOLUME_VOICE  CGameInstance::GetInstance()->GetVolume(CSound_Manager::SOUND_TYPE::SOUND_VOICE)

#define VTFID(p) p - OBJECT::TESTVTF_BEGIN
#define VTFENUM(p) p + OBJECT::TESTVTF_BEGIN

#define GENERICID(p) p - OBJECT::TESTGENERIC_BEGIN
#define GENERICENUM(p) p + OBJECT::TESTGENERIC_BEGIN

#define DEFAULT_COLOR { 1.f, 1.f, 1.f }
#define NORMAL_COLOR { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f }
#define ADVANCED_COLOR  { 50.f / 255.f, 230.f / 255.f, 80.f / 255.f }
#define RARE_COLOR  { 210.f / 255.f, 110.f / 255.f, 255.f / 255.f }
#define UNIQUE_COLOR  { 250.f / 255.f, 255.f / 255.f, 110.f / 255.f }
