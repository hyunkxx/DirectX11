#include "stdafx.h"
#include "..\Public\CityObject.h"

#include "GameInstance.h"

#include "GameMode.h"

CCityObject::CCityObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_CityObject_Desc, sizeof(SCITY_OBJECT_DESC));
}

CCityObject::CCityObject(const CCityObject & rhs)
	: CGameObject(rhs)
	, m_CityObject_Desc(rhs.m_CityObject_Desc)
{
}

HRESULT CCityObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCityObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_CityObject_Desc, pArg, sizeof(SCITY_OBJECT_DESC));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	SetUp_State(m_CityObject_Desc.PSA.vP, m_CityObject_Desc.PSA.vS, m_CityObject_Desc.PSA.vA);

	switch (m_CityObject_Desc.iSMD_ID)
	{
	case SMODEL::SMD_CIT_BUI_6:
		m_fCullingRatio = { 0.0f };
		break;
	case SMODEL::SMD_CIT_GRO_0:
		m_fCullingRatio = { 0.0f };
		break;
	default:
		m_fCullingRatio = { 20.0f };
		break;
	}

	m_fCullingRatio = { 0.0f };

	return S_OK;
}

void CCityObject::Start()
{
	__super::Start();

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
}

void CCityObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CCityObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	if (0.0f < m_fCullingRatio && false == pGameInstance->InWorldSpace(m_pMainTransform->Get_State(CTransform::STATE_POSITION), m_fCullingRatio))
		return;
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
		
}

HRESULT CCityObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	switch (m_CityObject_Desc.iOption_ID)
	{
	case Client::CCityObject::OP_DECO:
		if (FAILED(Render_Default()))
			return E_FAIL;
		break;
	case Client::CCityObject::OP_FLOOR:
		if (FAILED(Render_Floor()))
			return E_FAIL;
		break;
	case Client::CCityObject::OP_INTERACT:
		if (FAILED(Render_Interect()))
			return E_FAIL;
		break;
	case Client::CCityObject::OP_END:
		break;
	default:
		break;
	}

	switch (m_CityObject_Desc.iType_ID)
	{
	case Client::CCityObject::ID_BUI:
		break;
	case Client::CCityObject::ID_ELE:
		break;
	case Client::CCityObject::ID_RAI:
		break;
	case Client::CCityObject::ID_GRO:
		break;
	case Client::CCityObject::ID_STA:
		break;
	case Client::CCityObject::ID_DES:
		break;
	case Client::CCityObject::ID_STO:
		break;
	case Client::CCityObject::ID_END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CCityObject::RenderShadow()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(1);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCityObject::Render_Default()
{
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		
		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		m_iShaderPassID = 0;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCityObject::Render_Floor()
{
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		m_iShaderPassID = 12;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCityObject::Render_Interect()
{
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		if (CCityObject::TYPE_ID::ID_STO == m_CityObject_Desc.iType_ID)
		{
			if (1 == i)
			{
				m_CityObject_Desc.Use_Glow = true;
				m_iShaderPassID = 0;
			}
			else
			{
				m_CityObject_Desc.Use_Glow = false;
				m_iShaderPassID = 13;
			}

			if (FAILED(m_pShaderCom->SetRawValue("g_IsUseGlow", &m_CityObject_Desc.Use_Glow, sizeof(_bool))))
				return E_FAIL;
			
			if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
				return E_FAIL;

			m_IsNormalTex = false;

			m_pShaderCom->Begin(m_iShaderPassID);
			m_pModelCom->Render(i);
			
		}
		else
		{
			m_iShaderPassID = 0;

			if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
				return E_FAIL;

			m_IsNormalTex = false;

			m_pShaderCom->Begin(m_iShaderPassID);
			m_pModelCom->Render(i);

		}

	}

	return S_OK;
}

void CCityObject::SetUp_State(_float3 vP, _float3 vS, _float3 vA)
{
	m_CityObject_Desc.PSA.vP = vP;
	m_CityObject_Desc.PSA.vS = vS;
	m_CityObject_Desc.PSA.vA = vA;

	_float4		vPos = { vP.x, vP.y, vP.z, 1.0f };
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pMainTransform->Set_Scale(vS);
	m_pMainTransform->SetRotationXYZ(vA);
}

HRESULT CCityObject::Add_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, m_CityObject_Desc.iSMD_ID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


HRESULT CCityObject::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetRawValue("g_IsUseGlow", &m_CityObject_Desc.Use_Glow, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCityObject::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//if (nullptr == pGameInstance)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
	//	return E_FAIL;

	return S_OK;
}

CCityObject * CCityObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCityObject* pInstance = new CCityObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCityObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCityObject::Clone(void * pArg)
{
	CCityObject* pInstance = new CCityObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCityObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCityObject::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
