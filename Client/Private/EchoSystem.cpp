#include "stdafx.h"
#include "..\Public\EchoSystem.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

CEchoSystem::CEchoSystem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEchoSystem::CEchoSystem(const CEchoSystem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEchoSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEchoSystem::Initialize(void * pArg)
{
	ZeroMemory(m_BindEcho, sizeof m_BindEcho);
	ZeroMemory(m_Echos, sizeof m_Echos);

	// 정령
	m_Echos[EC_ZIGZAG].bActive = true;
	m_Echos[EC_ZIGZAG].bBind = false;
	m_Echos[EC_ZIGZAG].iGrade = 1;
	m_Echos[EC_ZIGZAG].iLevel = 1;
	m_Echos[EC_ZIGZAG].fAttack = 25.f;
	m_Echos[EC_ZIGZAG].iNameTextureID = STATIC_IMAGE::TEXT_EC_ZIGZAG;
	m_Echos[EC_ZIGZAG].iIconTextureID = STATIC_IMAGE::ICON_EC_ZIGZAG;

	m_Echos[EC_TICTAC].bActive = false;
	m_Echos[EC_TICTAC].bBind = false;
	m_Echos[EC_TICTAC].iGrade = 1;
	m_Echos[EC_TICTAC].iLevel = 1;
	m_Echos[EC_TICTAC].fAttack = 30.f;
	m_Echos[EC_TICTAC].iNameTextureID = STATIC_IMAGE::TEXT_EC_TICTAC;
	m_Echos[EC_TICTAC].iIconTextureID = STATIC_IMAGE::ICON_EC_TICTAC;

	m_Echos[EC_WHOOSH].bActive = false;
	m_Echos[EC_WHOOSH].bBind = false;
	m_Echos[EC_WHOOSH].iGrade = 1;
	m_Echos[EC_WHOOSH].iLevel = 1;
	m_Echos[EC_WHOOSH].fAttack = 30.f;
	m_Echos[EC_WHOOSH].iNameTextureID = STATIC_IMAGE::TEXT_EC_WHOOSH;
	m_Echos[EC_WHOOSH].iIconTextureID = STATIC_IMAGE::ICON_EC_WHOOSH;

	m_Echos[EC_SNIPSNAP].bActive = false;
	m_Echos[EC_SNIPSNAP].bBind = false;
	m_Echos[EC_SNIPSNAP].iGrade = 1;
	m_Echos[EC_SNIPSNAP].iLevel = 1;
	m_Echos[EC_SNIPSNAP].fAttack = 35.f;
	m_Echos[EC_SNIPSNAP].iNameTextureID = STATIC_IMAGE::TEXT_EC_SNIPSNAP;
	m_Echos[EC_SNIPSNAP].iIconTextureID = STATIC_IMAGE::ICON_EC_SNIPSNAP;

	// 인간형
	m_Echos[EC_RUPTURE_WARRIOR].bActive = false;
	m_Echos[EC_RUPTURE_WARRIOR].bBind = false;
	m_Echos[EC_RUPTURE_WARRIOR].iGrade = 2;
	m_Echos[EC_RUPTURE_WARRIOR].iLevel = 1;
	m_Echos[EC_RUPTURE_WARRIOR].fAttack = 75.f;
	m_Echos[EC_RUPTURE_WARRIOR].iNameTextureID = STATIC_IMAGE::TEXT_EC_RUPTURE_WARRIOR;
	m_Echos[EC_RUPTURE_WARRIOR].iIconTextureID = STATIC_IMAGE::ICON_EC_RUPTURE_WARRIOR;

	m_Echos[EC_FUSHION_WARRIOR].bActive = true;
	m_Echos[EC_FUSHION_WARRIOR].bBind = false;
	m_Echos[EC_FUSHION_WARRIOR].iGrade = 2;
	m_Echos[EC_FUSHION_WARRIOR].iLevel = 1;
	m_Echos[EC_FUSHION_WARRIOR].fAttack = 75.f;
	m_Echos[EC_FUSHION_WARRIOR].iNameTextureID = STATIC_IMAGE::TEXT_EC_FUSHION_WARRIOR;
	m_Echos[EC_FUSHION_WARRIOR].iIconTextureID = STATIC_IMAGE::ICON_EC_FUSHION_WARRIOR;

	// 프레데터
	m_Echos[EC_IMPULSE_PREDATOR].bActive = true;
	m_Echos[EC_IMPULSE_PREDATOR].bBind = false;
	m_Echos[EC_IMPULSE_PREDATOR].iGrade = 3;
	m_Echos[EC_IMPULSE_PREDATOR].iLevel = 1;
	m_Echos[EC_IMPULSE_PREDATOR].fAttack = 175.f;
	m_Echos[EC_IMPULSE_PREDATOR].iNameTextureID = STATIC_IMAGE::TEXT_EC_IMPULSE_PREDATOR;
	m_Echos[EC_IMPULSE_PREDATOR].iIconTextureID = STATIC_IMAGE::ICON_EC_IMPULSE_PREDATOR;

	m_Echos[EC_GLACIO_PREDATOR].bActive = false;
	m_Echos[EC_GLACIO_PREDATOR].bBind = false;
	m_Echos[EC_GLACIO_PREDATOR].iGrade = 3;
	m_Echos[EC_GLACIO_PREDATOR].iLevel = 1;
	m_Echos[EC_GLACIO_PREDATOR].fAttack = 175.f;
	m_Echos[EC_GLACIO_PREDATOR].iNameTextureID = STATIC_IMAGE::TEXT_EC_GLACIO_PREDATOR;
	m_Echos[EC_GLACIO_PREDATOR].iIconTextureID = STATIC_IMAGE::ICON_EC_GLACIO_PREDATOR;

	m_Echos[EC_PNENUMA_PREDATOR].bActive = true;
	m_Echos[EC_PNENUMA_PREDATOR].bBind = false;
	m_Echos[EC_PNENUMA_PREDATOR].iGrade = 3;
	m_Echos[EC_PNENUMA_PREDATOR].iLevel = 1;
	m_Echos[EC_PNENUMA_PREDATOR].fAttack = 175.f;
	m_Echos[EC_PNENUMA_PREDATOR].iNameTextureID = STATIC_IMAGE::TEXT_EC_PNENUMA_PREDATOR;
	m_Echos[EC_PNENUMA_PREDATOR].iIconTextureID = STATIC_IMAGE::ICON_EC_PNENUMA_PREDATOR;

	// 엘리트
	m_Echos[EC_ELITE_STONE].bActive = true;
	m_Echos[EC_ELITE_STONE].bBind = false;
	m_Echos[EC_ELITE_STONE].iGrade = 4;
	m_Echos[EC_ELITE_STONE].iLevel = 1;
	m_Echos[EC_ELITE_STONE].fAttack = 210.f;
	m_Echos[EC_ELITE_STONE].iNameTextureID = STATIC_IMAGE::TEXT_EC_ELITE_STONE;
	m_Echos[EC_ELITE_STONE].iIconTextureID = STATIC_IMAGE::ICON_EC_ELITE_STONE;

	m_Echos[EC_ELITE_FLUTE].bActive = true;
	m_Echos[EC_ELITE_FLUTE].bBind = false;
	m_Echos[EC_ELITE_FLUTE].iGrade = 4;
	m_Echos[EC_ELITE_FLUTE].iLevel = 1;
	m_Echos[EC_ELITE_FLUTE].fAttack = 210.f;
	m_Echos[EC_ELITE_FLUTE].iNameTextureID = STATIC_IMAGE::TEXT_EC_ELITE_FLUTE;
	m_Echos[EC_ELITE_FLUTE].iIconTextureID = STATIC_IMAGE::ICON_EC_ELITE_FLUTE;

	// 보스
	m_Echos[EC_CROWNLESS].bActive = true;
	m_Echos[EC_CROWNLESS].bBind = false;
	m_Echos[EC_CROWNLESS].iGrade = 5;
	m_Echos[EC_CROWNLESS].iLevel = 1;
	m_Echos[EC_CROWNLESS].fAttack = 325.f;
	m_Echos[EC_CROWNLESS].iNameTextureID = STATIC_IMAGE::TEXT_EC_CROWNLESS;
	m_Echos[EC_CROWNLESS].iIconTextureID = STATIC_IMAGE::ICON_EC_CROWNLESS;

	return S_OK;
}

void CEchoSystem::Start()
{
}

void CEchoSystem::PreTick(_double TimeDelta)
{
}

void CEchoSystem::Tick(_double TimeDelta)
{
}

void CEchoSystem::LateTick(_double TimeDelta)
{
}

HRESULT CEchoSystem::Render()
{
	return S_OK;
}

HRESULT CEchoSystem::RenderShadow()
{
	return S_OK;
}

void CEchoSystem::RenderGUI()
{
}

void CEchoSystem::ReleaseEcho(ECHO_SLOT eTargetCharacter)
{
	if (!m_BindEcho[eTargetCharacter])
		return;

	m_BindEcho[eTargetCharacter]->bBind = false;
	m_BindEcho[eTargetCharacter] = nullptr;
	m_BindEchoType[eTargetCharacter] = EC_EMPTY;
}

_float CEchoSystem::ComputeDamage(ECHO_SLOT eTargetCharacter)
{
	m_BindEcho[eTargetCharacter]->fAttack *= (m_BindEcho[eTargetCharacter]->iLevel * 0.75f);

	return m_BindEcho[eTargetCharacter]->fAttack;
}

_uint CEchoSystem::GetEchoLevel(ECHO_TYPE eEchoType)
{
	return m_Echos[eEchoType].iLevel;
}

_bool CEchoSystem::Levelup(ECHO_TYPE eEchoType)
{
	if (m_Echos[eEchoType].iLevel >= 3)
		return false;

	m_Echos[eEchoType].iLevel++;

	return true;
}

_bool CEchoSystem::BindEcho(ECHO_SLOT eTargetCharacter, ECHO_TYPE eEchoIndex)
{
	if (m_Echos[eEchoIndex].bBind)
		return false;

	if (m_BindEcho[eTargetCharacter] != nullptr)
		m_BindEcho[eTargetCharacter]->bBind = false;

	m_BindEcho[eTargetCharacter] = &m_Echos[eEchoIndex];
	m_BindEcho[eTargetCharacter]->bBind = true;
	m_BindEchoType[eTargetCharacter] = eEchoIndex;

	return true;
}

CEchoSystem * CEchoSystem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEchoSystem* pInstance = new CEchoSystem(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEchoSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEchoSystem::Clone(void * pArg)
{
	CEchoSystem* pInstance = new CEchoSystem(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEchoSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEchoSystem::Free()
{
	__super::Free();


}
