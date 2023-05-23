#include "stdafx.h"
#include "..\Public\ActionCam.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "CameraMovement.h"

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

	CCameraMovement* pCamMovement = nullptr;
	pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	if (!pCamMovement)
		return E_FAIL;

	//생성시 자동 등록
	pCamMovement->AddCamera(m_eCamType, this);
	m_bUse = false;

	return S_OK;
}

void CActionCam::Start()
{
	ActionInit();

}

void CActionCam::Tick(_double TimeDelta)
{
	if (false == m_bUse)
		return;
	
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
		vCurPos = XMVectorLerp(vCurPos, vBonePos + XMVector3Normalize(vTargetLook + -vTargetRight) * 1.f, (_float)TimeDelta * 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
		break;
	case 2:
		vCurPos = XMVectorLerp(vCurPos, vTargetPos - vTargetLook * 6.f, (_float)TimeDelta * 2.f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		if (XMVectorGetX(XMVector3Length(vCurPos - (vTargetPos - vTargetLook * 6.f))) < 0.1f)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			
			CCameraMovement* pCamMovement;
			pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
			pCamMovement->SetCamPosition(CCameraMovement::CAM_MAINPLAYER, vCurPos);

			pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
		}

		break;
	}

	m_pMainTransform->LookAt(vBonePos);

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
	switch (m_eCamType)
	{
	case CCameraMovement::CAM_BANGSUN:
		actionInit_Bangsun();
		break;
	}
}

void CActionCam::actionInit_Bangsun()
{
	//방순이 무브먼트 셋업
	StartVibration(8.f, 0.15f);

	_vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.35f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	m_iAction = 0;
	m_fTimeAcc = 0.f;
}
