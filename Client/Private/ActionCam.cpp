#include "stdafx.h"
#include "..\Public\ActionCam.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "CameraMovement.h"
#include "PlayerState.h"

CActionCam::CActionCam(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CActionCam::CActionCam(const CActionCam & rhs)
 	: CCamera(rhs)
	, m_eCamType(rhs.m_eCamType)
{
}

HRESULT CActionCam::Initialize_Prototype(CCameraMovement::CAM_TYPE eCamType)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eCamType = eCamType;

	return S_OK;
}

HRESULT CActionCam::Initialize(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(CCamera::Initialize(pArg)))
		return E_FAIL;

	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	if (!m_pCamMovement)
		return E_FAIL;

	//생성시 자동 등록
	m_pCamMovement->AddCamera(m_eCamType, this);
	m_bUse = false;

	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	if (!m_pPlayerState)
		return E_FAIL;
	
	return S_OK;
}

void CActionCam::Start()
{
}

void CActionCam::Tick(_double TimeDelta)
{
	if (false == m_bUse)
		return;

	switch (m_eCamType)
	{
	case CCameraMovement::CAM_BANGSUN:
		actionLoop_Bangsung(TimeDelta);
		break;
	case CCameraMovement::CAM_YANGYANG:
		actionLoop_Yangyang(TimeDelta);
		break;
	case CCameraMovement::CAM_CHIXIA:
		actionLoop_Chixia(TimeDelta);
		break;
	}

	__super::Tick(TimeDelta);
}

void CActionCam::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

void CActionCam::RenderGUI()
{
}

CActionCam * CActionCam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCameraMovement::CAM_TYPE eCamType)
{
	CActionCam* pInstance = new CActionCam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eCamType)))
	{
		MSG_BOX("Failed to Create : CActionCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CActionCam::Clone(void * pArg)
{
	CActionCam* pInstance = new CActionCam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CActionCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActionCam::Free()
{
	__super::Free();
	Safe_Release(m_pMainTransform);
}

void CActionCam::AttachTargetTransform(CTransform * pTransform)
{
	if (!pTransform)
		return;

	m_pTargetTransform = pTransform;
}

void CActionCam::ActionInit()
{
	m_pTargetTransform = m_pPlayerState->Get_ActiveCharacter()->GetTransform();

	switch (m_eCamType)
	{
	case CCameraMovement::CAM_BANGSUN:
		actionInit_Bangsun();
		break;
	case CCameraMovement::CAM_YANGYANG:
		actionInit_Yangyang();
		break;
	case CCameraMovement::CAM_CHIXIA:
		actionInit_Chixia();
		break;
	}
}

void CActionCam::RevertPrevCam(_double TimeDelta)
{
	//메인 플레이어 캠으로 되돌림
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	CCamera* pMainCam = m_pCamMovement->GetCamera(CCameraMovement::CAM_MAINPLAYER);
	CAMERA_DESC& PrevCamDesc = pMainCam->GetCamDesc();
	CAMERA_DESC& CurCamDesc = GetCamDesc();

	static _vector vEye, vAt;
	static _bool bCheck = false;
	if (!bCheck)
	{
		bCheck = true;
		vEye = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
		vAt = XMLoadFloat3(&CurCamDesc.vAt);
	}

	m_fRevertAcc += (_float)TimeDelta;
	static const _double dDuration = 0.5f;
	_double dRatio = m_fRevertAcc / dDuration;

	_vector vDestEye, vDestAt;
	vDestEye = XMVectorLerp(vEye, pMainCam->GetTransform()->Get_State(CTransform::STATE_POSITION), (_float)dRatio);
	vDestAt = XMVectorLerp(vAt, XMLoadFloat3(&PrevCamDesc.vAt), (_float)dRatio);

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vDestEye);
	m_pMainTransform->LookAt(vDestAt);

	if (m_fRevertAcc > dDuration)
	{
		bCheck = false;
		m_fRevertAcc = 0.f;
		m_pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
	}
}

void CActionCam::actionInit_Bangsun()
{
	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 1.f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}

void CActionCam::actionInit_Yangyang()
{
	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(-(m_pTargetTransform->Get_State(CTransform::STATE_RIGHT)) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 1.2f) * 0.8f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}

void CActionCam::actionInit_Chixia()
{
	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 1.3f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}

void CActionCam::actionLoop_Bangsung(_double TimeDelta)
{
	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 방순이 전용으로 추후에 수정해야함
	if (m_iAction < 2)
	{
		m_fTimeAcc += (_float)TimeDelta;

		if (m_iAction == 1)
		{
			if (!bSlowMotion && m_fTimeAcc > 0.4f)
			{
				bSlowMotion = true;
				pGameInstance->TimeSlowDown(0.125f, 0.1f, 10.f);
			}

			if (m_fTimeAcc > 0.8f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
		else
		{
			if (m_fTimeAcc > 0.5f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
	}

	_vector vBonePos = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition()
		, XMMatrixRotationY(180.f)), XMLoadFloat4x4(m_pTargetTransform->Get_WorldMatrixPtr()));

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	_vector vTargetRight = m_pTargetTransform->Get_State(CTransform::STATE_RIGHT);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 1.35f);

	switch (m_iAction)
	{
	case 0:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vLook + vTargetRight), (_float)TimeDelta * 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 1:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vTargetLook + -(vTargetRight * 1.5f)), (_float)TimeDelta * 0.75f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos - vTargetLook * 8.f, (_float)TimeDelta * 2.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos - vTargetLook * 8.f))) < 0.1f)
			m_iAction++;

		break;
	case 3:
		bSlowMotion = false;
		RevertPrevCam(TimeDelta);
		break;
	}

	if (m_iAction < 3)
	{
		XMStoreFloat3(&m_CameraDesc.vAt, vBonePos);
		m_pMainTransform->LookAt(vBonePos);
	}
}

void CActionCam::actionLoop_Yangyang(_double TimeDelta)
{
	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_iAction < 2)
	{
		m_fTimeAcc += (_float)TimeDelta;

		if (m_iAction == 1)
		{
			if (!bSlowMotion && m_fTimeAcc > 0.4f)
			{
				bSlowMotion = true;
				pGameInstance->TimeSlowDown(0.125f, 0.1f, 10.f);
			}

			if (m_fTimeAcc > 0.8f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
		else
		{
			if (m_fTimeAcc > 0.5f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
	}

	_vector vBonePos = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition()
		, XMMatrixRotationY(180.f)), XMLoadFloat4x4(m_pTargetTransform->Get_WorldMatrixPtr()));

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	_vector vTargetRight = m_pTargetTransform->Get_State(CTransform::STATE_RIGHT);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 1.35f);

	switch (m_iAction)
	{
	case 0:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vLook + vTargetRight), (_float)TimeDelta * 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 1:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vTargetLook + -(vTargetRight * 1.5f)), (_float)TimeDelta * 0.75f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos - vTargetLook * 8.f, (_float)TimeDelta * 3.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos - vTargetLook * 8.f))) < 0.1f)
			m_iAction++;

		break;
	case 3:
		bSlowMotion = false;
		RevertPrevCam(TimeDelta);
		break;
	}

	if (m_iAction < 3)
	{
		XMStoreFloat3(&m_CameraDesc.vAt, vBonePos);
		m_pMainTransform->LookAt(vBonePos);
	}
}

void CActionCam::actionLoop_Chixia(_double TimeDelta)
{
	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_iAction < 2)
	{
		m_fTimeAcc += (_float)TimeDelta;

		if (m_iAction == 1)
		{
			if (!bSlowMotion && m_fTimeAcc > 0.4f)
			{
				bSlowMotion = true;
				pGameInstance->TimeSlowDown(0.125f, 0.1f, 10.f);
			}

			if (m_fTimeAcc > 0.8f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
		else
		{
			if (m_fTimeAcc > 0.5f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
	}

	_vector vBonePos = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition()
		, XMMatrixRotationY(180.f)), XMLoadFloat4x4(m_pTargetTransform->Get_WorldMatrixPtr()));

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	_vector vTargetRight = m_pTargetTransform->Get_State(CTransform::STATE_RIGHT);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 1.35f);

	switch (m_iAction)
	{
	case 0:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vLook), (_float)TimeDelta * 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 1:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vTargetLook * 1.5f), (_float)TimeDelta * 0.75f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos - vTargetLook * 8.f, (_float)TimeDelta * 3.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos - vTargetLook * 8.f))) < 0.1f)
			m_iAction++;

		break;
	case 3:
		bSlowMotion = false;
		RevertPrevCam(TimeDelta);
		break;
	}

	if (m_iAction < 3)
	{
		XMStoreFloat3(&m_CameraDesc.vAt, vBonePos);
		m_pMainTransform->LookAt(vBonePos);
	}
}
