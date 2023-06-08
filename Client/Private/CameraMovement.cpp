#include "stdafx.h"
#include "..\Public\CameraMovement.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "Camera.h"
#include "ActionCam.h"

CCameraMovement::CCameraMovement(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCameraMovement::CCameraMovement(const CCameraMovement & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCameraMovement::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraMovement::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;
	
	for (int i = 0; i < CAM_END; ++i)
		m_pCams[i] = nullptr;

	return S_OK;
}

void CCameraMovement::Start()
{
}

void CCameraMovement::Tick(_double TimeDelta)
{
}

void CCameraMovement::LateTick(_double TimeDelta)
{
}

void CCameraMovement::RenderGUI()
{
}

HRESULT CCameraMovement::BindCamera(CAM_TYPE eCamType, CCamera * pCamera)
{
	m_pCams[eCamType] = pCamera;

	if (!m_pCams[eCamType])
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraMovement::BindTransform(CTransform * pTransform)
{
	assert(pTransform);

	if (!pTransform)
		return E_FAIL;

	m_pTargetTransform = pTransform;

	for (size_t i = 0; i < CAM_END; ++i)
	{
		if (m_pCams[i] == nullptr)
		{
			continue;
		}

		IAttachTargetTransform* pAttachment = dynamic_cast<IAttachTargetTransform*>(m_pCams[i]);
		if (pAttachment)
			pAttachment->AttachTargetTransform(m_pTargetTransform);
	}

	return S_OK;
}

HRESULT CCameraMovement::AddCamera(CAM_TYPE eCameraType, CCamera * pCamera)
{
 	if (!pCamera)
	{
		MSG_BOX("CCameraMovement::AddCam() : pCamera nullptr");
		return E_FAIL;
	}

	m_pCams[eCameraType] = pCamera;

	return S_OK;
}

void CCameraMovement::UseCamera(CAM_TYPE eCameraType)
{
	if (eCameraType != CAM_MAINPLAYER)
		m_pCams[CAM_MAINPLAYER]->StopShake();

	for (size_t i = 0; i < CAM_END; ++i)
	{
		if (m_pCams[i] == nullptr)
			continue;

		if (i == eCameraType)
		{
			m_pCams[i]->Set_Use(true);
			m_eCurCamType = eCameraType;

			if (eCameraType > CAM_UI && eCameraType < CAM_END)
				static_cast<CActionCam*>(m_pCams[eCameraType])->ActionInit();
		}
		else
		{
			m_pCams[i]->Set_Use(false);
		}
	}

}

void CCameraMovement::SetupBone(CAM_TYPE eCameraType, CBone * pBone)
{
	if (!pBone)
		return;

	if (eCameraType <= CAM_UI || eCameraType >= CAM_END)
		return;

	static_cast<CActionCam*>(m_pCams[eCameraType])->SetupBone(pBone);
}

void CCameraMovement::ResetPlayerCamera()
{
	if(m_pCams[CAM_MAINPLAYER])
		m_pCams[CAM_MAINPLAYER] = nullptr;
}

void CCameraMovement::StartWave()
{
	if (m_eCurCamType != CAM_MAINPLAYER)
		return;

	m_pCams[CAM_MAINPLAYER]->StartWave();
}

void CCameraMovement::StartWave(CCamera::SHAKE_DESC tagShakeDesc)
{
	if (m_eCurCamType != CAM_MAINPLAYER)
		return;

	m_pCams[CAM_MAINPLAYER]->StartWave(tagShakeDesc);
}

void CCameraMovement::StartVibration()
{
	if (m_eCurCamType != CAM_MAINPLAYER)
		return;

	m_pCams[CAM_MAINPLAYER]->StartVibration();
}

void CCameraMovement::StartVibration(_float fRange, _float fDuration)
{
	if (m_eCurCamType != CAM_MAINPLAYER)
		return;

	m_pCams[CAM_MAINPLAYER]->StartVibration(fRange, fDuration);
}

HRESULT CCameraMovement::addComponents()
{
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(0.3f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CCameraMovement * CCameraMovement::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCameraMovement* pInstance = new CCameraMovement(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCameraMovement");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCameraMovement::Clone(void * pArg)
{
	CCameraMovement* pInstance = new CCameraMovement(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCameraMovement");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraMovement::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
}