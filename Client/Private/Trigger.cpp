#include "stdafx.h"
#include "..\Public\Trigger.h"

#include "GameInstance.h"

#include "GameMode.h"

CTrigger::CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_TriggerDesc, sizeof(TRIGGER_DESC));
	m_TriggerDesc.fRange = 1.0f;
}

CTrigger::CTrigger(const CTrigger & rhs)
	: CGameObject(rhs)
	, m_TriggerDesc(rhs.m_TriggerDesc)
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_TriggerDesc, pArg, sizeof(TRIGGER_DESC));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eTriggerType = (CTrigger::TRIGGER_TYPE)m_TriggerDesc.iTriggerType;
	m_eTriggerID = (CTrigger::TRIGGER_ID)m_TriggerDesc.iTriggerID;

	SetUp_State();

	return S_OK;
}

void CTrigger::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTrigger::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
}

HRESULT CTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CTrigger::SetUp_State()
{
	_float4		vPos = { m_TriggerDesc.vP.x, m_TriggerDesc.vP.y, m_TriggerDesc.vP.z, 1.0f };
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pMainTransform->Set_Scale(m_TriggerDesc.vS);
	m_pMainTransform->SetRotationXYZ(m_TriggerDesc.vA);
}

HRESULT CTrigger::Add_Components()
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
	m_pMainTransform->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_POTAL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


HRESULT CTrigger::SetUp_ShaderResources()
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

CTrigger * CTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrigger::Clone(void * pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}