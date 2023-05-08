#include "stdafx.h"
#include "..\Public\IntroCamera.h"

#include "GameInstance.h"

CIntroCamera::CIntroCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CIntroCamera::CIntroCamera(const CIntroCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CIntroCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntroCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CIntroCamera::Start()
{
}

void CIntroCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//_vector vCurrentPos

	__super::Tick(TimeDelta);
}

void CIntroCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CIntroCamera::Render()
{
	return S_OK;
}

void CIntroCamera::RenderGUI()
{
}

HRESULT CIntroCamera::Add_Components()
{
	return S_OK;
}

HRESULT CIntroCamera::Setup_ShaderResources()
{
	return S_OK;
}

_float CIntroCamera::GetCameraSpeed()
{
	if (m_pMainTransform == nullptr)
		return 0.f;
	return m_pMainTransform->Get_Speed();
}

CIntroCamera* CIntroCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIntroCamera*	pInstance = new CIntroCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CIntroCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIntroCamera::Clone(void* pArg)
{
	CIntroCamera*	pInstance = new CIntroCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CIntroCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIntroCamera::Free()
{
	__super::Free();
}
