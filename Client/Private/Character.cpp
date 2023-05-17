#include "stdafx.h"
#include "..\Public\Character.h"

#include "GameMode.h"

CCharacter::PHYSICMOVE CCharacter::StatePhysics[SP_END]
{
	// (x, -z, y) 
	// �� ������
	{ true, true, _float3(0.f, -1.f, 0.f),	0.f, 0.f, 0.f, 0.f, 0.f },
	// �ȱ� _F, ���
	{ true, true, _float3(0.f, -1.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	// �޸��� _F, ���
	{ true, true, _float3(0.f, -1.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	// ���� _F, ���
	{ true, true, _float3(0.f, -1.f, 0.f),	7.5f, 0.f, 0.f, 0.f, 0.f },
	// ����+@ _F, ���
	{ true, true, _float3(0.f, -1.f, 0.f),	10.0f, 0.f, 0.f, 0.f, 0.f },
	// ����, ���� � ���� ����, xz������, y���ӵ� ����
	{ false, false, _float3(0.f, 0.f, 0.f), 0.f, 0.3f, 0.1f, 15.f * 0.016f, -15.f * 0.016f },
	// PLAYERGIRL_AIRATTACK, ���
	{ true, true, _float3(0.f, 0.f, -1.f), 70.f, 0.f, 0.f, 0.f, 0.f  }
	/*{},
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
