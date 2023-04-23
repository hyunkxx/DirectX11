#include "stdafx.h"
#include "..\Public\ModelTest_Static.h"

#include "GameInstance.h"

CModelTest_Static::CModelTest_Static(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelTest_Static::CModelTest_Static(const CModelTest_Static & rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelTest_Static::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelTest_Static::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CModelTest_Static::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGI = CGameInstance::GetInstance();
	pGI->AddCollider(m_pColliderCom, COLLISION_LAYER::COLL_BASE);
	m_pColliderCom->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

void CModelTest_Static::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CModelTest_Static::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);


		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CModelTest_Static::RenderGUI()
{
	ImGui::Begin("CamDistance");
	_float fDistance = ComputeCameraLength();
	ImGui::DragFloat("Distance", &fDistance);
	ImGui::End();
}

HRESULT CModelTest_Static::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed= XMConvertToRadians(90.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;
	m_pMainTransform->Set_Scale(_float3(0.01f, 0.01f, 0.01f));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SMODEL::SMD_TEST,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 4.f, 4.f, 4.f};
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelTest_Static::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CModelTest_Static * CModelTest_Static::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModelTest_Static* pInstance = new CModelTest_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CModelTest_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CModelTest_Static::Clone(void * pArg)
{
	CModelTest_Static* pInstance = new CModelTest_Static(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModelTest_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelTest_Static::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
