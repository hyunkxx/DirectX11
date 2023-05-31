#include "stdafx.h"
#include "..\Public\Character.h"

#include "GameMode.h"

_int CCharacter::Monindex = 0;
CCharacter::PHYSICMOVE CCharacter::StatePhysics[SP_END]
{
	// (x, -z, y) 
	// 안 움직임
	{ true, true, _float3(0.f, -1.f, 0.f),	0.f, 0.f, 0.f, 0.f, 0.f },
	// 걷기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	// 달리기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	// 질주 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	7.5f, 0.f, 0.f, 0.f, 0.f },
	// 질주+@ _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	10.0f, 0.f, 0.f, 0.f, 0.f },
	// 낙하, 직전 운동 상태 적용, xz감쇄율, y가속도 적용
	{ false, false, _float3(0.f, 0.f, 0.f), 0.f, 0.5f, 20.f, 20.f, -20.f},
	// PLAYERGIRL_AIRATTACK, 등속
	{ true, true, _float3(0.f, 0.f, -1.f), 70.f, 0.f, 0.f, 0.f, 0.f  },
	// BEHIT_FLY_START, 
	{ true, false, _float3(0.f, 0.2f, 1.f), 10.f, 0.5f, 20.f, 20.f, -20.f},
	// BEHIT_HOVER, 
	{ true, false, _float3(0.f, 0.2f, 1.f), 5.f, 0.5f, 20.f, 20.f, -20.f},
	// BEHIT_PUSH, 
	{ true, false, _float3(0.f, 1.f, 0.25f), 20.f, 0.5f, 20.f, 20.f, -20.f}
};

void CCharacter::Shot_SlowKey(_float fTargetTime, _float fLerpSpeed)
{
	CGameInstance* pGame = CGameInstance::GetInstance();
	pGame->TimeSlowDown(fTargetTime, fLerpSpeed);
}

void CCharacter::Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed)
{
	m_bDissolve = true;
	m_fDissolveTimeAcc = 0.f;

	m_bDissolveType = bDissolveType;
	m_fDissolveSpeed = fDissolveSpeed;
}

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

	m_vDissolveColor = { 1.f, 0.7f, 0.4f };

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
	// 디졸브 프레임 단위 처리
	if (m_bDissolve)
	{
		m_fDissolveTimeAcc += (_float)TimeDelta * m_fDissolveSpeed;
		if (2.f <= m_fDissolveTimeAcc)
		{
			m_bDissolve = false;
			m_fDissolveTimeAcc = 0.f;
		}

		if (m_bDissolveType)
			m_fDissolveAmount = 1.f - m_fDissolveTimeAcc;
		else
			m_fDissolveAmount = m_fDissolveTimeAcc;
	}
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

_vector CCharacter::Get_Position()
{
	return m_pMainTransform->Get_State(CTransform::STATE_POSITION);
}

void CCharacter::Push_OnNavi(_fvector PushVector)
{
	m_pMainTransform->Push_OnNavi(PushVector, static_cast<CNavigation*>(Find_Component(TEXT("Com_Navigation"))));
}


