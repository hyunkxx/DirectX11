#include "stdafx.h"
#include "..\Public\Tree_4.h"

#include "GameInstance.h"
#include "GameMode.h"

CTree_4::CTree_4(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTree_4::CTree_4(const CTree_4 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTree_4::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_4::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_EditionDesc, sizeof(SOBJECT_EDITION_DESC));

	if (nullptr != pArg)
	{
		memcpy(&m_EditionDesc, pArg, sizeof(SOBJECT_EDITION_DESC));

		if (FAILED(Load_EditionColor(m_EditionDesc.pEditionFilePath)))
			return E_FAIL;

		m_fCullingRatio = m_EditionDesc.fCullingRatio;
	}

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);
	m_iShaderPassID = 0;

	return S_OK;
}

void CTree_4::Start()
{
	__super::Start();

}

void CTree_4::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTree_4::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Culling(m_pMainTransform->Get_WorldMatrixInverse(), m_fCullingRatio);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
	}
}

HRESULT CTree_4::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(DrawDefault()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_4::DrawDefault()
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

		// 쉐이더에 색상을 던짐 && 던진 색상이 적용될 매쉬 번호
		if (true == m_UseEditionColor && i == m_iEditionColor_MeshNum)
		{
			// 그중 노말맵이 없는경우.
			if (true == m_IsDistinction_NormalTex)
			{
				m_iShaderPassID = 3;
				m_IsDistinction_NormalTex = false;
			}
			else
				m_iShaderPassID = 4;
		}
		else
		{
			if (true == m_IsDistinction_NormalTex)
			{
				m_iShaderPassID = 0;
				m_IsDistinction_NormalTex = false;
			}
			else
				m_iShaderPassID = 1;
		}

		m_pShaderCom->Begin(m_iShaderPassID);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTree_4::Load_EditionColor(const _tchar * pEditionColorFilePath)
{
	HANDLE		hFile = CreateFile(pEditionColorFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CTree_4 : EditionColor");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	ReadFile(hFile, &m_UseEditionColor, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_iEditionColor_MeshNum, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_vEditionColor, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTree_4::RenderShadow()
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

HRESULT CTree_4::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_TREE_4,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL_INSTANCE,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_4::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetRawValue("g_vEditionColor", &m_vEditionColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree_4::SetUp_ShadowShaderResources()
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

CTree_4 * CTree_4::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTree_4* pInstance = new CTree_4(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTree_4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTree_4::Clone(void * pArg)
{
	CTree_4* pInstance = new CTree_4(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTree_4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree_4::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
