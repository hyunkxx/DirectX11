#include "stdafx.h"
#include "..\Public\Missile_RotAround.h"

#include "GameMode.h"
#include "GameInstance.h"

CMissile_RotAround::CMissile_RotAround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMissile(pDevice, pContext)
{
}

CMissile_RotAround::CMissile_RotAround(const CMissile_RotAround & rhs)
	: CMissile(rhs)
{
}

HRESULT CMissile_RotAround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile_RotAround::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		ROTMISSILEDESC tRotMissileDesc;
		memcpy(&tRotMissileDesc, pArg, sizeof(ROTMISSILEDESC));

		if (FAILED(__super::Initialize(&tRotMissileDesc.tMissileDesc)))
			return E_FAIL;

		m_pTargetTransform = tRotMissileDesc.pTargetTransform;
		m_pTargetBone = tRotMissileDesc.pTargetBone;

		XMStoreFloat3(&m_vAxis, XMVector3Normalize(XMLoadFloat3(&tRotMissileDesc.vAxis)));
		
		m_fInitAngle = tRotMissileDesc.fInitAngle;
		m_fRotSpeed = tRotMissileDesc.fRotSpeed;
		m_fDistance = tRotMissileDesc.fDistance;
	}

	return S_OK;
}

void CMissile_RotAround::Tick(_double TimeDelta)
{
	m_fCurAngle += m_fRotSpeed * (_float)TimeDelta;
	if (m_fCurAngle > XMConvertToRadians(360.f))
		m_fCurAngle -= XMConvertToRadians(360.f);

	_vector vPosition = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition(), XMMatrixRotationY(180.f)), XMLoadFloat4x4(&m_pTargetTransform->Get_WorldMatrix()))
		+ XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vShotLookDir), XMMatrixRotationAxis(XMLoadFloat3(&m_vAxis), m_fCurAngle))) * m_fDistance;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPosition);

	__super::Tick(TimeDelta);
}

void CMissile_RotAround::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

_bool CMissile_RotAround::Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix)
{
	if (ACTIVE == m_eState)
		return false;

	XMStoreFloat3(&m_vShotLookDir, XMVector3Normalize(vLookDir));
	m_fCurAngle = m_fInitAngle;

	// 타겟 본 월드 위치 + Shot 시점 정면 벡터 > axis 회전 후 정규화 * 거리;
	_vector vRotInitPos = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition(), XMMatrixRotationY(180.f)), XMLoadFloat4x4(&m_pTargetTransform->Get_WorldMatrix()))
		+ XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vShotLookDir), XMMatrixRotationAxis(XMLoadFloat3(&m_vAxis), m_fInitAngle))) * m_fDistance;

	__super::Shot(vRotInitPos, vLookDir, vMissileRotMatrix);

	return true;
}

CMissile_RotAround * CMissile_RotAround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissile_RotAround* pInstance = new CMissile_RotAround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile_RotAround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMissile_RotAround::Clone(void * pArg)
{
	CMissile_RotAround* pInstance = new CMissile_RotAround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMissile_RotAround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile_RotAround::Free()
{
	__super::Free();
}