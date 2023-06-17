#include "stdafx.h"
#include "..\Public\Invisible_Chest.h"


#include "GameMode.h"
#include "GameInstance.h"
#include "AcquireSystem.h"
#include "P_PlayerGirl.h"

#include "ItemDB.h"
#include "Inventory.h"
#include "Effect.h"

CInvisible_Chest::CInvisible_Chest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractionObject(pDevice, pContext)
{
}

CInvisible_Chest::CInvisible_Chest(const CInvisible_Chest & rhs)
	: CInteractionObject(rhs)
	, m_eChestType(rhs.m_eChestType)
{
}

HRESULT CInvisible_Chest::Initialize_Prototype(CHEST_TYPE eChestType)
{
	if (FAILED(CInteractionObject::Initialize_Prototype()))
		return E_FAIL;

	m_eChestType = eChestType;

	return S_OK;
}

HRESULT CInvisible_Chest::Initialize(void * pArg)
{
	if (FAILED(CInteractionObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	if (pArg)
	{
		_float3 vPos = *(_float3*)pArg;
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
	}

	switch (m_eChestType)
	{
	case CHEST_SIMPLE:
		m_vColor = ADVANCED_COLOR;
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

	m_Invisible_Effect = CGameInstance::GetInstance()->Get_Effect(L"Invisible_Box_Effect", EFFECT_ID::COMON);
	
	if (nullptr == m_Invisible_Effect)
		return E_FAIL;

	m_Invisible_Effect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());

	return S_OK;
}

void CInvisible_Chest::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Inventory")));
}

void CInvisible_Chest::PreTick(_double TimeDelta)
{
}

void CInvisible_Chest::Tick(_double TimeDelta)
{

	if (CGameInstance::GetInstance()->InputKey(DIK_M) == KEY_STATE::TAP && STATE_INVISIBLE != m_ChestState && m_bRader_Detection)
	{
		m_Invisible_Effect = nullptr;
		m_Invisible_Effect = CGameInstance::GetInstance()->Get_Effect(L"Invisible_Box_Effect", EFFECT_ID::COMON);
		
		if (nullptr == m_Invisible_Effect)
			return;

		m_Invisible_Effect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());
		m_ChestState = STATE_INVISIBLE;
	}

	switch (m_ChestState)
	{
	case Client::CInvisible_Chest::STATE_INVISIBLE:
		Invisible_Tick(TimeDelta);
		break;
	case Client::CInvisible_Chest::STATE_APPEAR:
		Appear_Tick(TimeDelta);
		break;
	case Client::CInvisible_Chest::STATE_VISIVLE:
		Visivle_Tick(TimeDelta);
		break;
	default:
		break;
	}
}

void CInvisible_Chest::LateTick(_double TimeDelta)
{
	switch (m_ChestState)
	{
	case Client::CInvisible_Chest::STATE_INVISIBLE:
		Invisible_LateTick(TimeDelta);
		break;
	case Client::CInvisible_Chest::STATE_APPEAR:
		Appear_LateTick(TimeDelta);
		break;
	case Client::CInvisible_Chest::STATE_VISIVLE:
		Visivle_LateTick(TimeDelta);
		break;
	default:
		break;
	}
}

HRESULT CInvisible_Chest::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(SetupShaderResource()))
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

	if (STATE_VISIVLE == m_ChestState)
	{
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
	}

	if (m_fBurstRim > 0.f && m_bRimLight)
	{
		_float vRimPower = 10.f;
		//_float3 vColor = LEGEND_COLOR;
		_float4 vRimColor = _float4(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);
		if (FAILED(m_pShader->SetRawValue("g_RimPower", &vRimPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_RimColor", &vRimColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fBurstRim, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(11);
		m_pModel->Render(0);
	}

	return S_OK;
}

HRESULT CInvisible_Chest::RenderShadow()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(SetupShadowResource()))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		m_pShader->Begin(1);
		m_pModel->Render(i);
	}

	return S_OK;
}

void CInvisible_Chest::RenderGUI()
{
}

void CInvisible_Chest::Invisible_Tick(_double TimeDelta)
{
}

void CInvisible_Chest::Appear_Tick(_double TimeDelta)
{
	if (m_bRimLight)
	{
		m_fBurstRim += (_float)TimeDelta * 0.5f;
		if (m_fBurstRim >= 0.3f)
		{
			m_ChestState = STATE_VISIVLE;
		}
	}
}

void CInvisible_Chest::Visivle_Tick(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bRimLight)
	{
		m_fBurstRim -= (_float)TimeDelta * 0.25f;
		if (m_fBurstRim <= 0.f)
		{
			m_fBurstRim = 0.f;
			m_bRimLight = false;

			CEffect* pEffect = nullptr;

			switch (m_eChestType)
			{
			case CHEST_SIMPLE://Green
				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Box_Appear_Effect_G", EFFECT_ID::COMON);
				pEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());
				break;
			case CHEST_STANDARD: //Purple
				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Box_Appear_Effect_P", EFFECT_ID::COMON);
				pEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());
				break;
			case CHEST_EXPANDED:
				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Box_Appear_Effect_Y", EFFECT_ID::COMON);
				pEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());
				break;
			default:
				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Box_Appear_Effect_Y", EFFECT_ID::COMON);
				pEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix());
				break;
			}
		}
	}

	if (m_bOverlapedPlayer)
	{
		if (pGameInstance->InputKey(DIK_F) == KEY_STATE::HOLD && !m_bInteractionActive)
		{
			m_fPushButtonAcc += (_float)TimeDelta;
			pGameMode->SetGagebar(m_fPushButtonAcc);
			if (m_fPushButtonAcc > 1.f)
			{
				m_fPushButtonAcc = 0.f;
				m_bInteractionBegin = true;

				_float4x4 matrix = m_pMainTransform->Get_WorldMatrix();
				matrix._42 = matrix._42 + 1.f;
				CEffect* pOpenEffect = pGameInstance->Get_Effect(L"Get_Item_Effect_02", EFFECT_ID::COMON);
				pOpenEffect->Play_Effect(&matrix);
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
}

void CInvisible_Chest::Invisible_LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider , COLL_RIM_INTERACTION);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

}

void CInvisible_Chest::Appear_LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
}

void CInvisible_Chest::Visivle_LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
}

void CInvisible_Chest::Set_Position(_float3 vPos)
{
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

void CInvisible_Chest::Interaction(void * pArg)
{//interactionUIActive(false);

	CGameMode* pGameMode = CGameMode::GetInstance();

	//임시 아이템 처리
	switch (m_eChestType)
	{
	case CInvisible_Chest::CHEST_SIMPLE:
	{
		CItem::ITEM_DESC item0 = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item0.iAmount = 12900;
		CGameMode::GetInstance()->EnqueueItemDesc(item0);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item0.iAmount);
		break;
	}
	case CInvisible_Chest::CHEST_STANDARD:
	{
		CItem::ITEM_DESC item0 = CItemDB::GetInstance()->GetItemData(ITEM::COMMEMORATIVE_COIN);
		item0.iAmount = 2;
		CItem::ITEM_DESC item1 = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item1.iAmount = 1000;

		CGameMode::GetInstance()->EnqueueItemDesc(item0);
		CGameMode::GetInstance()->EnqueueItemDesc(item1);

		m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, item0.iAmount);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item1.iAmount);
		break;
	}
	case CInvisible_Chest::CHEST_EXPANDED:
	{
		CItem::ITEM_DESC item0 = CItemDB::GetInstance()->GetItemData(ITEM::TACTREITE_VOUCHER);
		item0.iAmount = 10;
		CItem::ITEM_DESC item1 = CItemDB::GetInstance()->GetItemData(ITEM::COMMEMORATIVE_COIN);
		item1.iAmount = 2;
		CItem::ITEM_DESC item2 = CItemDB::GetInstance()->GetItemData(ITEM::TACTITE_COIN);
		item2.iAmount = 500;
		CGameMode::GetInstance()->EnqueueItemDesc(item0);
		CGameMode::GetInstance()->EnqueueItemDesc(item1);
		CGameMode::GetInstance()->EnqueueItemDesc(item2);

		m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, item0.iAmount);
		m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, item1.iAmount);
		m_pInven->AddItem(ITEM::TACTITE_COIN, item2.iAmount);

		break;
	}
	default:
		break;
	}
}

HRESULT CInvisible_Chest::AddComponents()
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

HRESULT CInvisible_Chest::SetupShaderResource()
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

HRESULT CInvisible_Chest::SetupShadowResource()
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

void CInvisible_Chest::interactionUIActive(_bool bRender)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	switch (m_eChestType)
	{
	case CInvisible_Chest::CHEST_SIMPLE:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_SIMPLE_CHEST, bRender);
		break;
	case CInvisible_Chest::CHEST_STANDARD:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_STANDARD_CHEST, bRender);
		break;
	case CInvisible_Chest::CHEST_EXPANDED:
		pGameMode->SetInteractionActive(CInteractionUI::INTER_EXPANDED_CHEST, bRender);
		break;
	}
}

CInvisible_Chest * CInvisible_Chest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CHEST_TYPE eChestType)
{
	CInvisible_Chest* pInstance = new CInvisible_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eChestType)))
	{
		MSG_BOX("Failed to Create : CInvisible_Chest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInvisible_Chest::Clone(void * pArg)
{
	CInvisible_Chest* pInstance = new CInvisible_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInvisible_Chest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvisible_Chest::Free()
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

void CInvisible_Chest::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (m_ChestState != STATE_INVISIBLE)
		return;

	m_fBurstRim = 0.f;
	m_bRimLight = true;
	m_ChestState = STATE_APPEAR;

	m_Invisible_Effect->Shut_Down();
	m_Invisible_Effect = nullptr;
}

void CInvisible_Chest::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CGameMode* pGameMode = CGameMode::GetInstance();

	CP_PlayerGirl* pPlayer = dynamic_cast<CP_PlayerGirl*>(dest->GetOwner());
	if (pPlayer)
	{
		m_bOverlapedPlayer = true;
	}
}

void CInvisible_Chest::OnCollisionExit(CCollider * src, CCollider * dest)
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

void CInvisible_Chest::Off_Detection(void)
{
	m_bRader_Detection = false;

	if (nullptr != m_Invisible_Effect)
	{
		m_Invisible_Effect->Shut_Down();
		m_Invisible_Effect = nullptr;
	}

}

void CInvisible_Chest::Appear_Box(void)
{
	m_fBurstRim = 0.f;
	m_bRimLight = true;
	m_ChestState = STATE_APPEAR;
}
