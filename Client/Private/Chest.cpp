#include "stdafx.h"
#include "..\Public\Chest.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AcquireSystem.h"
#include "P_PlayerGirl.h"
#include "PlayerState.h"

#include "ItemDB.h"
#include "UI_Minimap.h"
#include "Inventory.h"
#include "Effect.h"

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
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	switch (m_eChestType)
	{
	case CHEST_SIMPLE:
		m_vColor = ADVANCED_COLOR;
		m_pMainTransform->SetRotation(vUp, XMConvertToRadians(90.f));
		break;
	case CHEST_STANDARD:
		m_vColor = UNIQUE_COLOR;
		break;
	case CHEST_EXPANDED:
		m_vColor = LEGEND_COLOR;
		break;
	default:
		m_vColor = DEFAULT_COLOR;
		break;
	}

	return S_OK;
}

void CChest::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//m_pUIIcon = static_cast<CUI_Minimap*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, TEXT("UI_Minimap")));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Inventory")));
	m_pState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("CharacterState")));
	//m_UIIndex = m_pUIIcon->Add_Icon(m_pMainTransform->Get_State(CTransform::STATE_POSITION), CUI_Minimap::BOX);
	//m_pUIIcon->SetRender(m_UIIndex, false);
}

void CChest::PreTick(_double TimeDelta)
{
}

void CChest::Tick(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bOverlapedPlayer)
	{
		if (pGameInstance->InputKey(DIK_F) == KEY_STATE::HOLD && !m_bInteractionActive)
		{
			//pGameMode->PlayEffect("Get_Item_Effect_01",COMMON)
			//pGameMode->PlayEffect("Get_Item_Effect_02",COMMON)
			//pGameInstance->Get_Effect(L"Get_Item_Effect_02", EFFECT_ID::COMON);

			//CEffect* pOpenEffect = pGameInstance->Get_Effect(L"Smoke_05_Gray", EFFECT_ID::COMON);
			//pOpenEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());

			m_fPushButtonAcc += (_float)TimeDelta;
			pGameMode->SetGagebar(m_fPushButtonAcc);
			if (m_fPushButtonAcc > 1.f)
			{
				m_fPushButtonAcc = 0.f;
				m_bInteractionBegin = true;

				_float4x4 matrix = m_pMainTransform->Get_WorldMatrix();
				matrix._42 = matrix._42 + 1.f;
				CEffect* pOpenEffect = pGameInstance->Get_Effect(L"Get_Item_Effect_02", EFFECT_ID::COMON);
				if(nullptr != pOpenEffect)
					pOpenEffect->Play_Effect(&matrix);

				//UI 추가
				///m_pUIIcon->SetRender(m_UIIndex, false);

				CGameInstance::GetInstance()->PlaySoundEx(L"DA_Au_Buff_Pickup_timerush.wem.wav",
					SOUND_UI_TEMP, 1.f);
			}
		}
		else
		{
			m_fPushButtonAcc = 0.f;
			pGameMode->SetGagebar(m_fPushButtonAcc);
		}
	}

	if (m_bInteractionBegin)
	{
		m_bInteractionBegin = false;
		m_bInteractionActive = true;

		m_fPushButtonAcc = 0.f;
		m_fTimeAcc = 0.f;
		Interaction();
	}

	if (!m_bInteractionActive)
	{
		m_fTimeAcc += (_float)TimeDelta * 0.25f;
		if (m_fTimeAcc > 1.f)
			m_fTimeAcc = 0.f;
	}
	else
	{
		m_fResetTimeAcc += (_float)TimeDelta;
		if (m_fResetTimeAcc > 3.f)
		{
			m_bInteractionActive = false;
			m_fResetTimeAcc = 0.f;
		}
	}

	if (m_bOverlapedPlayer && !m_bInteractionActive)
	{
		if (!m_bInteractionUIRender)
		{
			m_bInteractionUIRender = true;
			interactionUIActive(true);
		}
	}
	else
	{
		if (m_bInteractionUIRender)
		{
			m_bInteractionUIRender = false;
			interactionUIActive(false);
		}
	}

	//UI추가
	//if ((nullptr != m_pUIIcon)&&(true == this->IsActive()))
	//{
	//	if (false == m_pUIIcon->GetRenderState(m_UIIndex))
	//		m_pUIIcon->SetRender(m_UIIndex, true);
	//	m_pUIIcon->Set_ObjectPos(m_UIIndex, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	//	m_pUIIcon->SetRender(m_UIIndex, true);
	//}
}

void CChest::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
}

HRESULT CChest::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(setupShaderResource()))
		return E_FAIL;

	if (FAILED(m_pEmissive->Setup_ShaderResource(m_pShader, "g_EmissiveTexture")))
		return E_FAIL;
	if (FAILED(m_pSpecular->Setup_ShaderResource(m_pShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pSSAO->Setup_ShaderResource(m_pShader, "g_SSAOTexture")))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MASK_TWINKL, m_pShader, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vColor", &m_vColor, sizeof(_float3))))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pModel->Render(i);
	}

	return S_OK;
}

HRESULT CChest::RenderShadow()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(setupShadowResource()))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		m_pShader->Begin(1);
		m_pModel->Render(i);
	}

	return S_OK;
}

void CChest::RenderGUI()
{
}

void CChest::Interaction(void * pArg)
{
	//interactionUIActive(false);

	CGameMode* pGameMode = CGameMode::GetInstance();

	//임시 아이템 처리
	switch (m_eChestType)
	{
	case CChest::CHEST_SIMPLE:
	{
		CItem::ITEM_DESC item0 = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item0.iAmount = 500;
		CGameMode::GetInstance()->EnqueueItemDesc(item0);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item0.iAmount);

		CItem::ITEM_DESC item1 = CItemDB::GetInstance()->GetItemData(ITEM::CASKETPIECE);
		item0.iAmount = 5;
		CGameMode::GetInstance()->EnqueueItemDesc(item1);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item1.iAmount);
		break;
	}
	case CChest::CHEST_STANDARD:
	{
		CItem::ITEM_DESC item;

		item = CItemDB::GetInstance()->GetItemData(ITEM::COMMEMORATIVE_COIN);
		item.iAmount = 2;
		CGameMode::GetInstance()->EnqueueItemDesc(item);
		m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, item.iAmount);

		item = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item.iAmount = 1000;
		CGameMode::GetInstance()->EnqueueItemDesc(item);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item.iAmount);

		item = CItemDB::GetInstance()->GetItemData(ITEM::EXP1);
		item.iAmount = 10;
		CGameMode::GetInstance()->EnqueueItemDesc(item);
		m_pInven->AddItem(ITEM::EXP1, item.iAmount);
		break;
	}
	case CChest::CHEST_EXPANDED:
	{
		CItem::ITEM_DESC item0 = CItemDB::GetInstance()->GetItemData(ITEM::TACTREITE_VOUCHER);
		item0.iAmount = 10;
		CItem::ITEM_DESC item1 = CItemDB::GetInstance()->GetItemData(ITEM::COMMEMORATIVE_COIN);
		item1.iAmount = 2;
		CItem::ITEM_DESC item2 = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item2.iAmount = 500;
		CItem::ITEM_DESC item3 = CItemDB::GetInstance()->GetItemData(ITEM::EXP2);
		item2.iAmount = 10;
		CGameMode::GetInstance()->EnqueueItemDesc(item0);
		CGameMode::GetInstance()->EnqueueItemDesc(item1);
		CGameMode::GetInstance()->EnqueueItemDesc(item2);
		CGameMode::GetInstance()->EnqueueItemDesc(item3);

		m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, item0.iAmount);
		m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, item1.iAmount);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item2.iAmount);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item3.iAmount);
		m_pState->AddUnionExp(20.f);
		break;
	}
	default:
		break;
	}

	
	m_pState->AddUnionExp(20.f);
	m_pState->AddUnionExp(10.f);

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

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SIMPLE_BOX,
		TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::BOX_EMISSIVE,
		TEXT("Com_Texture_Emissive"), (CComponent**)&m_pEmissive)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::BOX_SPECULAR,
		TEXT("Com_Texture_Specular"), (CComponent**)&m_pSpecular)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::BOX_SSAO,
		TEXT("Com_Texture_SSAO"), (CComponent**)&m_pSSAO)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.0f, 0.f };
	CollDesc.vExtents = { 1.5f, 1.5f, 1.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	m_pCollider->SetSameObjectDetection(false);

	return S_OK;
}

HRESULT CChest::setupShaderResource()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CChest::setupShadowResource()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

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
	Safe_Release(m_pEmissive);
	Safe_Release(m_pSpecular);
	Safe_Release(m_pSSAO);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);

	Safe_Release(m_pCollider);
}

void CChest::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CChest::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	CP_PlayerGirl* pPlayer = dynamic_cast<CP_PlayerGirl*>(dest->GetOwner());
	if (pPlayer)
	{
		m_bOverlapedPlayer = true;
	}
}

void CChest::OnCollisionExit(CCollider * src, CCollider * dest)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	CP_PlayerGirl* pPlayer = dynamic_cast<CP_PlayerGirl*>(dest->GetOwner());
	if (pPlayer)
	{
		m_bOverlapedPlayer = false;
		interactionUIActive(false);
		pGameMode->SetGagebar(0.f);
	}
}
