#include "stdafx.h"
#include "..\Public\AcquireSystem.h"

#include "GameMode.h"
#include "GameInstance.h"

#include "InteractionUI.h"
#include "Inventory.h"
#include "ItemDB.h"
#include "P_PlayerGirl.h"

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
	CGameMode* pGameMode = CGameMode::GetInstance();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_pInteractionUI = static_cast<CInteractionUI*>(pGameInstance->Clone_GameObject(OBJECT::UI_INTERACTION));
	m_pAcquireUI = static_cast<CAcquireUI*>(pGameInstance->Clone_GameObject(OBJECT::UI_ACQUIRE));

	m_pInteractionUI->Start();
	m_pAcquireUI->Start();
	m_pInventory = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));

	pGameMode->SetupAcquireSystem(this);

	return S_OK;
}

void CAcquireSystem::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
}

void CAcquireSystem::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	m_pInteractionUI->Tick(TimeDelta);
	m_pAcquireUI->Tick(TimeDelta);

	if (pGameInstance->InputKey(DIK_INSERT) == KEY_STATE::TAP)
	{
		CItemDB* pDB = CItemDB::GetInstance();
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::TACTREITE_VOUCHER);
		
		m_pInventory->PushItemDesc(itemDesc);
	}
}

void CAcquireSystem::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	m_pInteractionUI->LateTick(TimeDelta);
	m_pAcquireUI->LateTick(TimeDelta);
}

HRESULT CAcquireSystem::Render()
{

	return S_OK;
}

void CAcquireSystem::RenderGUI()
{
	m_pInteractionUI->RenderGUI();
	m_pAcquireUI->RenderGUI();
}

void CAcquireSystem::SetInteractionActive(CInteractionUI::INTERACT_TYPE eType, _bool bValue)
{
	m_pInteractionUI->SetInteractionActive(eType, bValue);
}

void CAcquireSystem::EnqueueItemDesc(CItem::ITEM_DESC ItemDesc)
{
	m_pAcquireUI->EnqueueItemDesc(ItemDesc);
}

void CAcquireSystem::SetGageBar(_float fValue)
{
	m_pInteractionUI->SetGagebar(fValue);
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

	Safe_Release(m_pInteractionUI);
	Safe_Release(m_pAcquireUI);
}