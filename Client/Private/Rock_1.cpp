#include "stdafx.h"
#include "..\Public\Rock_1.h"

#include "GameInstance.h"
#include "GameMode.h"

CRock_1::CRock_1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRock_1::CRock_1(const CRock_1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRock_1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock_1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_EditionDesc, sizeof(SOBJECT_EDITION_DESC));

	if (nullptr != pArg)
	{
		memcpy(&m_EditionDesc, pArg, sizeof(SOBJECT_EDITION_DESC));

		if (FAILED(Load_DiffuseTexID(m_EditionDesc.pEditionFilePath)))
			return E_FAIL;

		m_fCullingRatio = m_EditionDesc.fCullingRatio;
	}

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);
	m_iShaderPassID = 0;

	return S_OK;
}

void CRock_1::Start()
{
	__super::Start();

}

void CRock_1::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CRock_1::LateTick(_double TimeDelta)
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

HRESULT CRock_1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(DrawDefault()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock_1::DrawDefault()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
		//return E_FAIL;

		m_pDiffuseTexture[m_iDiffuseTex_ID]->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture");

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

HRESULT CRock_1::Load_DiffuseTexID(const _tchar * pDiffuseTexIDFilePath)
{
	HANDLE		hFile = CreateFile(pDiffuseTexIDFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CRock_1 : DiffuseTexID");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	ReadFile(hFile, &m_iDiffuseTex_ID, sizeof(_uint), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CRock_1::RenderShadow()
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

HRESULT CRock_1::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_ROCK_1,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL_INSTANCE,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_1,
		TEXT("com_texture_d_1"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_2,
		TEXT("com_texture_d_2"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_3,
		TEXT("com_texture_d_3"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_4,
		TEXT("com_texture_d_4"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_4])))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock_1::SetUp_ShaderResources()
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

HRESULT CRock_1::SetUp_ShadowShaderResources()
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

CRock_1 * CRock_1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRock_1* pInstance = new CRock_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRock_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRock_1::Clone(void * pArg)
{
	CRock_1* pInstance = new CRock_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRock_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRock_1::Free()
{
	__super::Free();

	for (_uint i = 0; i < ROCK_DIFFUSE_KINDS::RD_END; i++)
		Safe_Release(m_pDiffuseTexture[i]);

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
