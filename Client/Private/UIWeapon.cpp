#include "stdafx.h"
#include "..\Public\UIWeapon.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Bone.h"

#include "UICharacter.h"
#include "PlayerState.h"

CUIWeapon::CUIWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUIWeapon::CUIWeapon(const CUIWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIWeapon::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWeapon::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(addComponents()))
		return E_FAIL;
	
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f)));
	m_pMainTransform->Set_WorldMatrix(WorldMatrix);
	
	CGameInstance* pGI = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGI->Find_GameObject(LEVEL_STATIC, L"CharacterState"));

	m_fDissolveAmount = 0.f;

	return S_OK;
}

void CUIWeapon::Start()
{
}

void CUIWeapon::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CUIWeapon::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	__super::Tick(TimeDelta);

	_matrix ParentMatrix = XMLoadFloat4x4(&m_pPropBone[0]->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(XMConvertToRadians(180.f));
	XMStoreFloat4x4(&m_WorldMatrix[0], XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * ParentMatrix * XMLoadFloat4x4(&m_pParentTransform->Get_WorldMatrix()));
	ParentMatrix = XMLoadFloat4x4(&m_pPropBone[1]->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(XMConvertToRadians(180.f));
	XMStoreFloat4x4(&m_WorldMatrix[1], XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * ParentMatrix * XMLoadFloat4x4(&m_pParentTransform->Get_WorldMatrix()));
	ParentMatrix = XMLoadFloat4x4(&m_pPropBone[2]->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(XMConvertToRadians(180.f));
	XMStoreFloat4x4(&m_WorldMatrix[2], XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * ParentMatrix * XMLoadFloat4x4(&m_pParentTransform->Get_WorldMatrix()));
	ParentMatrix = XMLoadFloat4x4(&m_pPropBone[3]->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(XMConvertToRadians(180.f));
	XMStoreFloat4x4(&m_WorldMatrix[3], XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * ParentMatrix * XMLoadFloat4x4(&m_pParentTransform->Get_WorldMatrix()));

	CItem::ITEM_DESC* pWeapon = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurPlayer);

	switch (pWeapon->iItemID)
	{
	case ITEM::SWORD0:
		m_iWeaponModel = SWORD1;
		break;
	case ITEM::SWORD1:
		m_iWeaponModel = SWORD2;
		break;
	case ITEM::SWORD2:
		m_iWeaponModel = SWORD3;
		break;
	case ITEM::SWORD3:
		m_iWeaponModel = SWORD4;
		break;
	case ITEM::SWORD4:
		m_iWeaponModel = SWORD5;
		break;
	case ITEM::GUN0:
		m_iWeaponModel = GUN1;
		break;
	case ITEM::GUN1:
		m_iWeaponModel = GUN2;
		break;
	case ITEM::GUN2:
		m_iWeaponModel = GUN3;
		break;
	case ITEM::GUN3:
		m_iWeaponModel = GUN4;
		break;
	case ITEM::GUN4:
		m_iWeaponModel = GUN5;
		break;
	}
	
	if (m_bOnHand)
	{
		m_fDissolveAmount -= (_float)TimeDelta;
		if (m_fDissolveAmount < -1.f)
		{
			m_bDessolve = true;
			m_fDissolveAmount = -1.f;
		}
	}
	else
	{
		if (m_bDessolve)
		{
			m_bDessolve = false;
			m_fDissolveAmount = 0.f;
		}

		m_fDissolveAmount += (_float)TimeDelta;
		if (m_fDissolveAmount > 1.f)
			m_fDissolveAmount = 1.f;
	}


}

void CUIWeapon::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
}

HRESULT CUIWeapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(setupShaderResources()))
		return E_FAIL;

	if (FAILED(renderWeapon()))
		return E_FAIL;

	return S_OK;
}

void CUIWeapon::SetupWeaponProp(_uint iCharacter, _uint iWeaponPropID, CBone * pBone)
{
	m_iCurPlayer = iCharacter;
	m_pPropBone[iWeaponPropID] = pBone;
}

HRESULT CUIWeapon::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("shader_model"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_1_MAIN,
		TEXT("model_sword1_main"), (CComponent**)&m_pModels[SWORD1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_1_SUB,
		TEXT("model_sword1_sub"), (CComponent**)&m_pModels[SWORD1_SUB])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_2_MAIN,
		TEXT("model_sword2_main"), (CComponent**)&m_pModels[SWORD2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_2_SUB,
		TEXT("model_sword2_sub"), (CComponent**)&m_pModels[SWORD2_SUB])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_3_MAIN,
		TEXT("model_sword3_main"), (CComponent**)&m_pModels[SWORD3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_3_SUB,
		TEXT("model_sword3_sub"), (CComponent**)&m_pModels[SWORD3_SUB])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_4_MAIN,
		TEXT("model_sword4_main"), (CComponent**)&m_pModels[SWORD4])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_4_SUB,
		TEXT("model_sword4_sub"), (CComponent**)&m_pModels[SWORD4_SUB])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_5_MAIN,
		TEXT("model_sword5_main"), (CComponent**)&m_pModels[SWORD5])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_5_SUB,
		TEXT("model_sword5_sub"), (CComponent**)&m_pModels[SWORD5_SUB])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_GUN_1,
		TEXT("model_gun1"), (CComponent**)&m_pModels[GUN1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_GUN_2,
		TEXT("model_gun2"), (CComponent**)&m_pModels[GUN2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_GUN_3,
		TEXT("model_gun3"), (CComponent**)&m_pModels[GUN3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_GUN_4,
		TEXT("model_gun4"), (CComponent**)&m_pModels[GUN4])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_GUN_5,
		TEXT("model_gun5"), (CComponent**)&m_pModels[GUN5])))
		return E_FAIL;


	return S_OK;
}

HRESULT CUIWeapon::setupShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWeapon::renderWeapon()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Ä®
	if (m_bOnHand)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[1])))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[3])))
			return E_FAIL;
	}

	_float3 vColor;
	switch (m_iCurPlayer)
	{
	case ROVER:
		vColor = LEGEND_COLOR;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MASK_DESSOLVE, m_pShader, "g_DissolveTexture")))
			return E_FAIL;

		break;
	case YANGYANG:
		vColor = { 0.15f, 0.15f, 0.8f };
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MASK_DESSOLVE, m_pShader, "g_DissolveTexture")))
			return E_FAIL;

		break;
	case CHIXIA:
		vColor = { 0.8f, 0.15f, 0.15f };
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MASK_DESSOLVE, m_pShader, "g_DissolveTexture")))
			return E_FAIL;

		break;
	}

	if (FAILED(m_pShader->SetRawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vDessolveColor", &vColor, sizeof(_float3))))
		return E_FAIL;

	_uint iNumMeshes = m_pModels[m_iWeaponModel]->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModels[m_iWeaponModel]->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);

		if (FAILED(m_pModels[m_iWeaponModel]->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShader->Begin(9);

		m_pModels[m_iWeaponModel]->Render(i);
	}

	//Ä®Áý
	if (m_iCurPlayer != CHIXIA)
	{
		if (m_bOnHand)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[0])))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[2])))
				return E_FAIL;
		}

		iNumMeshes = m_pModels[m_iWeaponModel + 5]->Get_NumMeshes();
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModels[m_iWeaponModel + 5]->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);

			if (FAILED(m_pModels[m_iWeaponModel + 5]->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
				return E_FAIL;

			m_pShader->Begin(9);
			m_pModels[m_iWeaponModel + 5]->Render(i);
		}
	}
	else
	{
		if (m_bOnHand)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[1])))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix[3])))
				return E_FAIL;
		}

		_uint iNumMeshes = m_pModels[m_iWeaponModel]->Get_NumMeshes();
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModels[m_iWeaponModel]->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);

			if (FAILED(m_pModels[m_iWeaponModel]->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
				return E_FAIL;

			m_pShader->Begin(9);
			m_pModels[m_iWeaponModel]->Render(i);
		}
	}

	return S_OK;
}

CUIWeapon * CUIWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUIWeapon*	pInstance = new CUIWeapon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUIWeapon::Clone(void * pArg)
{
	CUIWeapon*	pInstance = new CUIWeapon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	if (IsClone())
	{
		Safe_Release(m_pModels[0]);
		Safe_Release(m_pModels[1]);
		Safe_Release(m_pModels[2]);
		Safe_Release(m_pModels[3]);
		Safe_Release(m_pModels[4]);

		Safe_Release(m_pModels[5]);
		Safe_Release(m_pModels[6]);
		Safe_Release(m_pModels[7]);
		Safe_Release(m_pModels[8]);
		Safe_Release(m_pModels[9]);

		Safe_Release(m_pModels[10]);
		Safe_Release(m_pModels[11]);
		Safe_Release(m_pModels[12]);
		Safe_Release(m_pModels[13]);
		Safe_Release(m_pModels[14]);
	}

}
