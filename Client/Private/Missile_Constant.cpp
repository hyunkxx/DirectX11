#include "stdafx.h"
#include "..\Public\Missile_Constant.h"

#include "GameMode.h"
#include "GameInstance.h"

CMissile_Constant::CMissile_Constant(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMissile(pDevice, pContext)
{
}

CMissile_Constant::CMissile_Constant(const CMissile_Constant & rhs)
	: CMissile(rhs)
{
}

HRESULT CMissile_Constant::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile_Constant::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		CONSTMISSILEDESC tConstMissileDesc;
		memcpy(&tConstMissileDesc, pArg, sizeof(CONSTMISSILEDESC));

		if (FAILED(__super::Initialize(&tConstMissileDesc.tMissileDesc)))
			return E_FAIL;

		XMStoreFloat3(&m_vLocalMoveDir, XMVector3Normalize(XMLoadFloat3(&tConstMissileDesc.vFixMoveDir)));

		m_fVelocity = tConstMissileDesc.fVelocity;
		m_bMove = true;

		m_StopTime = tConstMissileDesc.StopTime;

		m_iStopCondition = tConstMissileDesc.iStopCondition;

		m_bTargetDir = tConstMissileDesc.bTargetDir;
	}

	return S_OK;
}

void CMissile_Constant::Tick(_double TimeDelta)
{
	m_StopTimer -= TimeDelta;
	if (0.0 > m_StopTimer)
		m_bMove = false;

	if(true == m_bMove)
	{
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, m_pMainTransform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vWorldMoveDir) * m_fVelocity * (_float)TimeDelta);
	}

	__super::Tick(TimeDelta);
}

void CMissile_Constant::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

_bool CMissile_Constant::Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vMoveDir)
{
	_bool bShot = __super::Shot(vInitPos, vLookDir, vMissileRotMatrix, vMoveDir);
	
	if (false == bShot)
		return false;
	
	m_StopTimer = m_StopTime;
	m_bMove = true;

	if (false == m_bTargetDir)
	{
		// 이동 방향
		_vector vLook = XMVector3Normalize(vLookDir);
		_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
		XMStoreFloat3(&m_vWorldMoveDir, XMVector3Normalize(vRight * m_vLocalMoveDir.x + vUp * m_vLocalMoveDir.y + vLook * m_vLocalMoveDir.z));
	}
	else
	{
		XMStoreFloat3(&m_vWorldMoveDir, vMoveDir);
		m_pMainTransform->Set_LookDir(vMoveDir);
	}
	
	return true;
}

void CMissile_Constant::OnCollisionOnTarget(CCollider * src, CCollider * dest)
{
	__super::OnCollisionOnTarget(src, dest);

	//if (STOP_ONCOLLISIONENTER == m_iStopCondition)
	//	m_bMove = false;
}

CMissile_Constant * CMissile_Constant::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissile_Constant* pInstance = new CMissile_Constant(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile_Constant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMissile_Constant::Clone(void * pArg)
{
	CMissile_Constant* pInstance = new CMissile_Constant(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMissile_Constant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile_Constant::Free()
{
	__super::Free();
}