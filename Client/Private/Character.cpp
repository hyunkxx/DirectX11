#include "stdafx.h"
#include "..\Public\Character.h"

#include "GameMode.h"

CCharacter::PHYSICMOVE CCharacter::PlayerStatePhysics[PSP_END]
{
	// (x, -z, y) 
	// 걷기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(0.f, 1.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(-1.f, 0.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(1.f, 0.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	// 달리기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(0.f, 1.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(-1.f, 0.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(1.f, 0.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	// 질주 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	5.f, 0.f, 0.f, 0.f, 0.f },
	{ true, true, _float3(0.f, -1.f, 0.f),	5.5f, 0.f, 0.f, 0.f, 0.f },
	/*{},
	{},
	{},
	{},
	{},
	{},
	{}*/
};


CCharacter::CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter::Initialize(void * pArg)
{
	ZeroMemory(&m_Scon, sizeof StateController);
	return S_OK;
}

void CCharacter::Start()
{
}

void CCharacter::Tick(_double TimeDelta)
{
}

void CCharacter::LateTick(_double TimeDelta)
{
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

HRESULT CCharacter::RenderShadow()
{
	return S_OK;
}

void CCharacter::RenderGUI()
{
}

void CCharacter::Free()
{
	__super::Free();
}
