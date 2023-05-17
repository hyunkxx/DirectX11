#include "stdafx.h"
#include "..\Public\Chest.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AcquireSystem.h"
#include "P_PlayerGirl.h"

#include "ItemDB.h"

CChest::CChest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractionObject(pDevice, pContext)
{
}

CChest::CChest(const CChest & rhs)
	: CInteractionObject(rhs)
	, m_eChestType(rhs.m_eChestType)
{
}

HRESULT CChest::Initialize_Prototype(CHEST_TYPE eChestType)
{
	if (FAILED(CInteractionObject::Initialize_Prototype()))
		return E_FAIL;

	m_eChestType = eChestType;

	return S_OK;
}

HRESULT CChest::Initialize(void * pArg)
{
	if (FAILED(CInteractionObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	if (pArg)
	{
		_float3 vPos = *(_float3*)pArg;
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
	}

	return S_OK;
}

void CChest::Start()
{
}

void CChest::Tick(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bOverlapedPlayer && pGameInstance->InputKey(DIK_F) == KEY_STATE::TAP)
		Interaction();

}

void CChest::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
}

HRESULT CChest::Render()
{
	return S_OK;
}

HRESULT CChest::RenderShadow()
{
	return S_OK;
}

void CChest::RenderGUI()
{
}

void CChest::Interaction(void * pArg)
{
	m_pCollider->SetActive(false);
	m_bOverlapedPlayer = false;

	interactionUIActive(false);

	//CItem::ITEM_DESC item = CItemDB::GetInstance()->GetItemData("item_cook");
	//CGameMode::GetInstance()->EnqueueItemDesc(item);
	//CGameMode::GetInstance()->EnqueueItemDesc(item);
	//CGameMode::GetInstance()->EnqueueItemDesc(item);
}

HRESULT CChest::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);
	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.5f, 0.f };
	CollDesc.vExtents = { 0.5f, 0.5f, 0.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	m_pCollider->SetSameObjectDetection(false);

	return S_OK;
}

void CChest::interactionUIActive(_bool bRender)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	switch (m_eChestType)
	{
	case CChest::CHEST_SIMPLE:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_SIMPLE_CHEST, bRender);
		break;
	case CChest::CHEST_STANDARD:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_STANDARD_CHEST, bRender);
		break;
	case CChest::CHEST_EXPANDED:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_EXPANDED_CHEST, bRender);
		break;
	}
}

CChest * CChest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CHEST_TYPE eChestType)
{
	CChest* pInstance = new CChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eChestType)))
	{
		MSG_BOX("Failed to Create : CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChest::Clone(void * pArg)
{
	CChest* pInstance = new CChest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest::Free()
{
	CInteractionObject::Free();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pShader);
}

void CChest::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	CP_PlayerGirl* pPlayer = dynamic_cast<CP_PlayerGirl*>(dest->GetOwner());
	if (pPlayer)
	{
		m_bOverlapedPlayer = true;
		interactionUIActive(true);
	}
}

void CChest::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CChest::OnCollisionExit(CCollider * src, CCollider * dest)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	CP_PlayerGirl* pPlayer = dynamic_cast<CP_PlayerGirl*>(dest->GetOwner());
	if (pPlayer)
	{
		m_bOverlapedPlayer = false;
		interactionUIActive(false);
	}
}
