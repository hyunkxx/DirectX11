#include "stdafx.h"
#include "..\Public\AcquireUI.h"

#include "GameMode.h"
#include "GameInstance.h"

CAcquireUI::CAcquireUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAcquireUI::CAcquireUI(const CAcquireUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAcquireUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAcquireUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	return S_OK;
}

void CAcquireUI::Start()
{
}

void CAcquireUI::Tick(_double TimeDelta)
{
	if (m_ItemDescQueue.empty())
		return;

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

}

void CAcquireUI::LateTick(_double TimeDelta)
{
	if (m_ItemDescQueue.empty())
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	if (!m_ItemDescQueue.empty())
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
	else
		MSG_BOX("item Input");
}

HRESULT CAcquireUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return S_OK;
}

void CAcquireUI::RenderGUI()
{
}

void CAcquireUI::SetAcquireActive(_bool bValue)
{
}

HRESULT CAcquireUI::addComponents()
{
	return S_OK;
}

CAcquireUI * CAcquireUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAcquireUI*	pInstance = new CAcquireUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAcquireUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAcquireUI::Clone(void * pArg)
{
	CAcquireUI*	pInstance = new CAcquireUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAcquireUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAcquireUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}
