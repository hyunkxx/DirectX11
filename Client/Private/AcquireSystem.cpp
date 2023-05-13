#include "stdafx.h"
#include "..\Public\AcquireSystem.h"

#include "GameMode.h"
#include "GameInstance.h"

#include "InteractionUI.h"

CAcquireSystem::CAcquireSystem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAcquireSystem::CAcquireSystem(const CAcquireSystem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAcquireSystem::Initialize_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAcquireSystem::Initialize(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	// Á¾¼ÓÀû UI
	m_pInteraction = static_cast<CInteractionUI*>(pGameInstance->Clone_GameObject(OBJECT::UI_INTERACTION));

	return S_OK;
}

void CAcquireSystem::Start()
{
	m_pInteraction->Start();
}

void CAcquireSystem::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	m_pInteraction->Tick(TimeDelta);
}

void CAcquireSystem::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	m_pInteraction->LateTick(TimeDelta);
}

HRESULT CAcquireSystem::Render()
{

	return S_OK;
}

void CAcquireSystem::RenderGUI()
{
	m_pInteraction->RenderGUI();

}

CAcquireSystem * CAcquireSystem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAcquireSystem*	pInstance = new CAcquireSystem(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAcquireSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAcquireSystem::Clone(void * pArg)
{
	CAcquireSystem*	pInstance = new CAcquireSystem(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAcquireSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAcquireSystem::Free()
{
	__super::Free();

	Safe_Release(m_pInteraction);
}