#include "stdafx.h"
#include "..\Public\MapObject.h"

#include "GameInstance.h"
#include "GameMode.h"

CMapObject::CMapObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
}

CMapObject::CMapObject(const CMapObject & rhs)
	: CGameObject(rhs)
	, m_EditionDesc(rhs.m_EditionDesc)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_EditionDesc, pArg, sizeof(SMAP_OBJECT_EDITION_DESC));
	}

	if (FAILED(Load_Edition()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);

	//m_EditionDesc.fCullingRatio = 0.0f;

	return S_OK;
}

void CMapObject::Start()
{
	__super::Start();
}

void CMapObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pModelCom && 0.0f < m_EditionDesc.fCullingRatio)
		m_pModelCom->Culling(m_pMainTransform->Get_WorldMatrixInverse(), m_EditionDesc.fCullingRatio);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	}
}

HRESULT CMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	switch (m_EditionDesc.iTypeID)
	{
	case CMapObject::MAPOBJECT_TYPEID::ID_TREE:
		if (FAILED(Render_EditionColor()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_ROCK:
		if (FAILED(Render_Rock()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_FLOOR:
		if (FAILED(Render_Default()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STAIRS:
		if (FAILED(Render_Default()))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CMapObject::RenderShadow()
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

HRESULT CMapObject::Render_EditionColor()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsDistinction_NormalTex 가 true 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsDistinction_NormalTex)))
			return E_FAIL;

		if (true == m_EditionDesc.UseEditionColor && i == m_EditionDesc.iEditionColor_MeshNum)
		{
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

HRESULT CMapObject::Render_Rock()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pDiffuseTexture[m_EditionDesc.iDiffuseTex_ID]->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture");

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

HRESULT CMapObject::Render_Default()
{
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
			m_iShaderPassID = 5;
			m_IsDistinction_NormalTex = false;
		}
		else
			m_iShaderPassID = 6;

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Load_Edition()
{
	switch (m_EditionDesc.iTypeID)
	{
	case CMapObject::MAPOBJECT_TYPEID::ID_TREE:
		if (FAILED(Load_EditionColor()))
		{
			m_EditionDesc.UseEditionColor = { false };
			m_EditionDesc.iEditionColor_MeshNum = { 0 };
			m_EditionDesc.vEditionColor = { _float3(1.0f, 1.0f, 1.0f) };
		}
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_ROCK:
		if (FAILED(Load_DiffuseTexID()))
		{
			m_EditionDesc.iDiffuseTex_ID = { 0 };
		}
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_FLOOR:
		if (FAILED(Load_EditionID()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STAIRS:
		if (FAILED(Load_EditionID()))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CMapObject::Load_EditionColor()
{
	HANDLE		hFile = CreateFile(m_EditionDesc.pEditionFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CMapObject : EditionColor");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	ReadFile(hFile, &m_EditionDesc.iSIMD_ID, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_EditionDesc.iTypeID, sizeof(_uint), &dwByte, nullptr);

	ReadFile(hFile, &m_EditionDesc.UseEditionColor, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_EditionDesc.iEditionColor_MeshNum, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_EditionDesc.vEditionColor, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMapObject::Load_DiffuseTexID()
{
	HANDLE		hFile = CreateFile(m_EditionDesc.pEditionFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CMapObject : DiffuseTexID");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	ReadFile(hFile, &m_EditionDesc.iSIMD_ID, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_EditionDesc.iTypeID, sizeof(_uint), &dwByte, nullptr);

	ReadFile(hFile, &m_EditionDesc.iDiffuseTex_ID, sizeof(_uint), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMapObject::Load_EditionID()
{
	HANDLE		hFile = CreateFile(m_EditionDesc.pEditionFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CMapObject : EditionID");
		return S_OK;
	}

	DWORD		dwByte = 0;

	ReadFile(hFile, &m_EditionDesc.iSIMD_ID, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_EditionDesc.iTypeID, sizeof(_uint), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMapObject::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, m_EditionDesc.iSIMD_ID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
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


HRESULT CMapObject::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetRawValue("g_vEditionColor", &m_EditionDesc.vEditionColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::SetUp_ShadowShaderResources()
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

CMapObject * CMapObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapObject* pInstance = new CMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapObject::Clone(void * pArg)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	for (_uint i = 0; i < ROCK_DIFFUSE_KINDS::RD_END; i++)
		Safe_Release(m_pDiffuseTexture[i]);

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
