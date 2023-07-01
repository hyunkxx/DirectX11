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
	
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(405.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pSubTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform_origin"), (CComponent**)&m_pOriginTransform, &TransformDesc)))
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
	case CCameraMovement::CAM_CROWN1:
		actionLoop_Crown1(TimeDelta);
		break;
	case CCameraMovement::CAM_CROWN2:
		actionLoop_Crown2(TimeDelta);
		break;
	}

	//_vector vDir = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK));
	//_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	//m_pOriginTransform->Set_WorldMatrix(m_pMainTransform->Get_WorldMatrix());

	//_matrix vMat = XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * XMMatrixRotationAxis(vDir, XMConvertToRadians(15.f));
	//m_pMainTransform->Set_WorldMatrix(vMat);

	__super::Tick(TimeDelta);
	//m_pMainTransform->Set_WorldMatrix(m_pOriginTransform->Get_WorldMatrix());
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
	Safe_Release(m_pSubTransform);
	Safe_Release(m_pOriginTransform);
}

void CActionCam::AttachTargetTransform(CTransform * pTransform)
{
	if (!pTransform)
		return;

	m_pTargetTransform = pTransform;
}

void CActionCam::ActionInit()
{
	switch (m_eCamType)
	{
	case CCameraMovement::CAM_BANGSUN:
		m_pTargetTransform = m_pPlayerState->Get_ActiveCharacter()->GetTransform();
		actionInit_Bangsun();
		break;
	case CCameraMovement::CAM_YANGYANG:
		m_pTargetTransform = m_pPlayerState->Get_ActiveCharacter()->GetTransform();
		actionInit_Yangyang();
		break;
	case CCameraMovement::CAM_CHIXIA:
		m_pTargetTransform = m_pPlayerState->Get_ActiveCharacter()->GetTransform();
		actionInit_Chixia();
		break;
	case CCameraMovement::CAM_CROWN1:
		actionInit_Crown1();
		break;
	case CCameraMovement::CAM_CROWN2:
		actionInit_Crown2();
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
	m_pSubTransform->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());

	m_iAction = 0;
	m_fTimeAcc = 0.f;
	m_fChixiaValue = 2.f;
}

void CActionCam::actionInit_Crown1()
{
	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 1.3f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pSubTransform->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}

void CActionCam::actionInit_Crown2()
{
	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 1.3f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pSubTransform->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}

void CActionCam::actionLoop_Bangsung(_double TimeDelta)
{
	TimeDelta *= 1.25f;

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
	TimeDelta *= 1.25f;

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
	TimeDelta *= 1.25f;

	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_iAction < 2)
	{
		m_fTimeAcc += (_float)TimeDelta;
		if (m_iAction == 1)
		{
			if (m_fTimeAcc > 0.8f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
		else
		{
			if (!bSlowMotion)
			{
				bSlowMotion = true;
				CRenderSetting::RGB_SPLIT_DESC Split;
				Split.m_fDistortion = 0.1f;
				Split.m_fStrength = 0.2f;
				Split.m_fSeparation = 0.5f;
				pGameInstance->SetSplitDesc(Split);
				pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DIR::SPLIT_DEFAULT, 0.8f);
			}

			_vector vSrc = XMVectorSet(m_fChixiaValue, m_fChixiaValue, m_fChixiaValue, 1.f);
			_vector vDes = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			m_fChixiaValue = XMVectorGetX(XMVectorLerp(vSrc, vDes, (_float)TimeDelta * 2.f));
			
			m_pSubTransform->Rotate(VECTOR_UP, -(TimeDelta * m_fChixiaValue));
			if (m_fTimeAcc > 1.3f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
		}
	}
	else
	{
		m_fTimeAcc += (_float)TimeDelta;
		if (m_fTimeAcc > 1.7f)
		{
			m_fTimeAcc = 0.f;
			m_iAction++;
		}
	}

	_vector vBonePos = XMVector3TransformCoord(XMVector3TransformCoord(m_pTargetBone->Get_CombinedPosition()
		, XMMatrixRotationY(180.f)), XMLoadFloat4x4(m_pTargetTransform->Get_WorldMatrixPtr()));

	_matrix pBoneMatrix = XMLoadFloat4x4(&m_pTargetBone->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(180.f) * XMLoadFloat4x4(m_pTargetTransform->Get_WorldMatrixPtr());

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	_vector vTargetRight = m_pTargetTransform->Get_State(CTransform::STATE_RIGHT);
	_vector vBoneDir1 = XMVector3Normalize(pBoneMatrix.r[2]);

	//vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 1.35f);
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 4.f);
	vBonePos = XMVectorSetY(vBonePos, XMVectorGetY(vBonePos) + 0.12f);

	_vector vSubPos = m_pSubTransform->Get_State(CTransform::STATE_POSITION);
	vSubPos = XMVectorSetY(vSubPos, XMVectorGetY(vSubPos) + 0.7f + (m_fTimeAcc * 0.45f));
	_vector vSubDir = m_pSubTransform->Get_State(CTransform::STATE_LOOK);

	switch (m_iAction)
	{
	case 0:
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vSubPos - vSubDir * (0.7f * m_fTimeAcc));
		break;
	case 1:

		break;
	case 2:
		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos - vTargetLook * 9.f))) > 0.1f)
		{
			vCurPos = XMVectorLerp(vCurPos, vTargetPos - vTargetLook * 9.f, (_float)TimeDelta * 4.f);
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
		}

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

void CActionCam::actionLoop_Crown1(_double TimeDelta)
{
	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_iAction < 2)
	{
		m_fTimeAcc += (_float)TimeDelta;
		switch (m_iAction)
		{
		case 0:
			if (m_fTimeAcc > 3.5f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
			break;
		case 1:
			if (m_fTimeAcc > 1.f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
			break;
		}
	}
	
	_vector vBonePos = XMVectorSet(m_pCrownBoneMatrix->_41, m_pCrownBoneMatrix->_42, m_pCrownBoneMatrix->_43, m_pCrownBoneMatrix->_44);

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = vBonePos;
	_vector vTargetLook = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_LOOK));
	_vector vTargetLook2 = XMVector3Normalize(XMVectorSet(m_pCrownBoneMatrix->_31, m_pCrownBoneMatrix->_32, m_pCrownBoneMatrix->_33, m_pCrownBoneMatrix->_34));
	_vector vTargetRight = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_RIGHT));

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos));

	m_vBonePos = vTargetPos;

	switch (m_iAction)
	{
	case 0:
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.3f);
		vCurPos = XMVectorLerp(vCurPos, vTargetPos + (vTargetLook + (-vTargetRight * 0.6f)) * 2.2f, (_float)TimeDelta * 2.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 1:
		//vTargetPos + (vTargetLook + vTargetRight) * 3.f 왼쪽
		m_vBonePos = XMVectorLerp(m_vBonePos, vTargetPos + XMVector3Normalize(vTargetLook) * (0.8f + (m_fTimeAcc * 1.f)), (_float)TimeDelta * 3.f);
		vCurPos = XMVectorLerp(vCurPos, vTargetPos + (vTargetLook + (-vTargetRight * 2.f)) * 1.2f, (_float)TimeDelta * 3.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos + vTargetLook * 12.f, (_float)TimeDelta * 4.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		if (!bSlowMotion)
		{
			bSlowMotion = true;
			CRenderSetting::RGB_SPLIT_DESC Split;
			Split.m_fDistortion = 0.8f;
			Split.m_fStrength = 0.3f;
			Split.m_fSeparation = 0.3f;
			pGameInstance->SetSplitDesc(Split);
			pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DIR::SPLIT_DEFAULT, 0.8f);
		}

		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos + vTargetLook * 12.f))) < 0.1f)
			m_iAction++;

		break;
	case 3:
		bSlowMotion = false;
		RevertPrevCam(TimeDelta);
		break;
	}

	if (m_iAction < 3)
	{
		XMStoreFloat3(&m_CameraDesc.vAt, m_vBonePos);
		m_pMainTransform->LookAt(m_vBonePos);
	}
}

void CActionCam::actionLoop_Crown2(_double TimeDelta)
{
	static _bool bSlowMotion = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	static _float m_fAcc = 0.f;

	if (m_iAction < 5)
	{
		m_fTimeAcc += (_float)TimeDelta;
		switch (m_iAction)
		{
		case 0:
			if (m_fTimeAcc > 3.f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}

			break;
		case 1:
			if (m_fTimeAcc > 2.f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}

			break;
		case 2:
			if (m_fTimeAcc > 3.f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}

			break;
		case 3:
			if (m_fTimeAcc > 1.f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}

			if (m_fTimeAcc >= 0.5f)
			{
				if (!bSlowMotion)
				{
					bSlowMotion = true;
					pGameInstance->StartBlackWhite(2.f);
				}
			}
			break;
		case 4:
			if (m_fTimeAcc > 1.5f)
			{
				m_fTimeAcc = 0.f;
				m_iAction++;
			}
			break;
		}
	}

	_vector vBonePos = XMVectorSet(m_pCrownBoneMatrix->_41, m_pCrownBoneMatrix->_42, m_pCrownBoneMatrix->_43, m_pCrownBoneMatrix->_44);

	_vector vLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	_vector vTargetRight = m_pTargetTransform->Get_State(CTransform::STATE_RIGHT);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 1.35f);

	switch (m_iAction)
	{
	case 0:
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vTargetLook + (-vTargetRight * 4.f)), (_float)TimeDelta * 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 1:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos + XMVector3Normalize(-vTargetRight) * 4.f, (_float)TimeDelta * 3.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos, (_float)TimeDelta * 0.3f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 3:
	case 4:
		if (m_fTimeAcc < 1.f)
		{
			vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
			vTargetPos = XMVectorSetY(vTargetPos, 20.f);
			vCurPos = XMVectorLerp(vCurPos, vTargetPos + (vTargetLook) * 3.f, (_float)TimeDelta * 3.f);
		}
		else
		{
			vCurPos = XMVectorLerp(vCurPos, vBonePos + (vTargetLook + -vTargetRight) * 1.3f, (_float)TimeDelta * 2.5f);
		}

		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		break;
	case 5:
		bSlowMotion = false;
		RevertPrevCam(TimeDelta);
		break;
	}

	if (m_iAction < 5)
	{
		XMStoreFloat3(&m_CameraDesc.vAt, vBonePos);
		m_pMainTransform->LookAt(vBonePos);
	}
}
