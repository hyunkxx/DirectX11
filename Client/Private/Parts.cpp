#include "stdafx.h"
#include "..\Public\Parts.h"

#include "GameMode.h"
#include "GameInstance.h"

CParts::CParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CParts::CParts(const CParts & rhs)
	: CGameObject(rhs)
	, m_iModelID(rhs.m_iModelID)
{
}

HRESULT CParts::Initialize_Prototype(_uint iModelID)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iModelID = iModelID;

	return S_OK;
}

HRESULT CParts::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_tPartsDesc, pArg, sizeof(PARTSDESC));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;


	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f)));

	m_pMainTransform->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

void CParts::Tick(_double TimeDelta)
{
	if (true != m_bRender)
		return;

	__super::Tick(TimeDelta);

	_matrix ParentMatrix = /*XMLoadFloat4x4(&m_tPartsDesc.ppParentBone[m_iCurParent]->Get_OffsetMatrix()) **/
		XMLoadFloat4x4(&m_tPartsDesc.ppParentBone[m_iCurParent]->Get_CombinedTransfromationMatrix()) *
		XMMatrixRotationY(XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * ParentMatrix * XMLoadFloat4x4(&m_tPartsDesc.pParentTransform->Get_WorldMatrix()));
}

void CParts::LateTick(_double TimeDelta)
{
	if (true != m_bRender)
		return;

	__super::LateTick(TimeDelta);
}

HRESULT CParts::Render()
{
	if (true != m_bRender)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
		//m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		if(m_bOutline)
			m_pShaderCom->Begin(2);
		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;

}

HRESULT CParts::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(Setup_ShadowShaderResource()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(1);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CParts::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, m_iModelID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts::Setup_ShadowShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	return S_OK;
}

CParts * CParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iModelID)
{
	CParts* pInstance = new CParts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iModelID)))
	{
		MSG_BOX("Failed to Create : CParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParts::Clone(void * pArg)
{
	CParts* pInstance = new CParts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParts::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
