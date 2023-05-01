#include "stdafx.h"
#include "..\Public\Tree_0.h"

#include "GameInstance.h"

CTree_0::CTree_0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTree_0::CTree_0(const CTree_0 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTree_0::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_0::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);
	m_iShaderPassID = 0;

	return S_OK;
}

void CTree_0::Start()
{
	__super::Start();

}

void CTree_0::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGI = CGameInstance::GetInstance();
	pGI->AddCollider(m_pColliderCom, COLLISION_LAYER::COLL_BASE);
	m_pColliderCom->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
}

void CTree_0::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pModelCom)
		m_pModelCom->Culling(m_pTransformCom->Get_WorldMatrixInverse(), 15.0f);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
	}
}

HRESULT CTree_0::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(DrawDefault()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_0::DrawDefault()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsDistinction_NormalTex 가 true 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsDistinction_NormalTex)))
			return E_FAIL;

		if (true == m_IsDistinction_NormalTex)
		{
			m_iShaderPassID = 0;
			m_IsDistinction_NormalTex = false;
		}
		else
			m_iShaderPassID = 1;

		m_pShaderCom->Begin(m_iShaderPassID);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTree_0::RenderShadow()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTree_0::Add_Components()
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
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	m_pTransformCom->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_0,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODEL_INSTANCE,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// 원점확인용으로 냅뒀음
	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 4.f, 4.f, 4.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_0::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_0::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	return S_OK;
}

CTree_0 * CTree_0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTree_0* pInstance = new CTree_0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTree_0");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTree_0::Clone(void * pArg)
{
	CTree_0* pInstance = new CTree_0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTree_0");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree_0::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
