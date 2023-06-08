#include "stdafx.h"
#include "..\Public\UICam.h"

#include "GameInstance.h"
#include "GameMode.h"

CUICam::CUICam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CUICam::CUICam(const CUICam& rhs)
	: CCamera(rhs)
	, m_eCamType(rhs.m_eCamType)
{
}

HRESULT CUICam::Initialize_Prototype(CCameraMovement::CAM_TYPE eCamType)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eCamType = eCamType;

	return S_OK;
}

HRESULT CUICam::Initialize(void* pArg)
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

	return S_OK;
}

void CUICam::Start()
{
}

void CUICam::Tick(_double TimeDelta)
{
	if (false == m_bUse)
		return;

	__super::Tick(TimeDelta);
}

void CUICam::LateTick(_double TimeDelta)
{
	if (false == m_bUse)
		return;

	__super::LateTick(TimeDelta);
}

HRESULT CUICam::Render()
{
	return S_OK;
}

void CUICam::RenderGUI()
{
}

_float CUICam::GetCameraSpeed()
{
	if (m_pMainTransform == nullptr)
		return 0.f;
	return m_pMainTransform->Get_Speed();
}

CUICam* CUICam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCameraMovement::CAM_TYPE eCamType)
{
	CUICam*	pInstance = new CUICam(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eCamType)))
	{
		MSG_BOX("Failed to Created : CUICam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUICam::Clone(void* pArg)
{
	CUICam*	pInstance = new CUICam(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUICam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUICam::Free()
{
	__super::Free();
}
