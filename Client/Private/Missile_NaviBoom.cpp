#include "stdafx.h"
#include "..\Public\Missile_NaviBoom.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Effect.h"
#include "MissilePool.h"

CMissile_NaviBoom::CMissile_NaviBoom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMissile_Constant(pDevice, pContext)
{
}

CMissile_NaviBoom::CMissile_NaviBoom(const CMissile_NaviBoom & rhs)
	: CMissile_Constant(rhs)
{
}

HRESULT CMissile_NaviBoom::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile_NaviBoom::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		NAVIBOOMMISSILEDESC tDesc;

		memcpy(&tDesc, pArg, sizeof NAVIBOOMMISSILEDESC);

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		m_pNavigation = tDesc.pNavigation;
	}

	return S_OK;
}

void CMissile_NaviBoom::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMissile_NaviBoom::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fHeight = m_pNavigation->Compute_Height(vPos);

	if (XMVectorGetY(vPos) < fHeight)
	{
		m_bNaviBoom = true;
		End();
	}
}

_bool CMissile_NaviBoom::Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vMoveDir)
{
	__super::Shot(vInitPos, vLookDir, vMissileRotMatrix, vMoveDir);

	m_bNaviBoom = false;

	return true;
}

void CMissile_NaviBoom::End()
{
	if (nullptr != m_tMissileDesc.ppNextMissilePool && true == m_bNaviBoom)
		(*m_tMissileDesc.ppNextMissilePool)->Shot(m_pMainTransform->Get_State(CTransform::STATE_POSITION),
			m_pMainTransform->Get_State(CTransform::STATE_LOOK), XMMatrixIdentity());

	if (nullptr != m_pEffect )
	{
		if (false == m_tMissileDesc.bNoShutDownEffect)
			m_pEffect->Shut_Down();
		m_pEffect = nullptr;
	}


	m_pCollider->SetActive(false);

	SetState(DISABLE);
}

CMissile_NaviBoom * CMissile_NaviBoom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissile_NaviBoom* pInstance = new CMissile_NaviBoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile_NaviBoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMissile_NaviBoom::Clone(void * pArg)
{
	CMissile_NaviBoom* pInstance = new CMissile_NaviBoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMissile_NaviBoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile_NaviBoom::Free()
{
	__super::Free();
}