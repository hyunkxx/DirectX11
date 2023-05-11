#include "stdafx.h"
#include "..\Public\CharacterSelect_Terrain.h"

#include "GameInstance.h"

CCharacterSelect_Terrain::CCharacterSelect_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacterSelect_Terrain::CCharacterSelect_Terrain(const CCharacterSelect_Terrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacterSelect_Terrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacterSelect_Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iShader_PassID = { 4 };

	if (FAILED(Load_UVSamplerRatio_Data(TEXT("../../Data/Lobby/Terrain/UVSamplerRatio.data"))))
	{
		m_fUVSampler_Ratio_1 = { 100.0f };
		m_fUVSampler_Ratio_2 = { 100.0f };
		m_fUVSampler_Ratio_3 = { 100.0f };
		m_fUVSampler_Ratio_4 = { 100.0f };
	}

	return S_OK;
}

void CCharacterSelect_Terrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
}

void CCharacterSelect_Terrain::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);

#ifdef _DEBUG
	// 디버그 모드 랜더링 안할 시 릭 나옴
	if (nullptr != m_pRenderer && nullptr != m_pNavigation)
	{
		m_pNavigation->Set_IntervalY(0.01f);
		m_pRenderer->AddDebugBundle(m_pNavigation);
	}
#endif // _DEBUG
}

HRESULT CCharacterSelect_Terrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(m_iShader_PassID);
	m_pVIBuffer->Render();

	return S_OK;
}

void CCharacterSelect_Terrain::RenderGUI()
{
}

HRESULT CCharacterSelect_Terrain::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("com_transform"), (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_CHARACTERSELECT_TERRAIN,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::VTXNORTEX,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::NAVIGATION,
		TEXT("com_navigation"), (CComponent**)&m_pNavigation)))
		return E_FAIL;

#pragma region TERRAIN_TEX
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_5,
		TEXT("com_texture_diffuse_1"), (CComponent**)&m_pDiffuseTexture[T_1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_6,
		TEXT("com_texture_diffuse_2"), (CComponent**)&m_pDiffuseTexture[T_2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_7,
		TEXT("com_texture_diffuse_3"), (CComponent**)&m_pDiffuseTexture[T_3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_8,
		TEXT("com_texture_diffuse_4"), (CComponent**)&m_pDiffuseTexture[T_4])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_N_5,
		TEXT("com_texture_n_1"), (CComponent**)&m_pNormalTexture[TERRAIN_KINDS::T_1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_N_6,
		TEXT("com_texture_n_2"), (CComponent**)&m_pNormalTexture[TERRAIN_KINDS::T_2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_N_7,
		TEXT("com_texture_n_3"), (CComponent**)&m_pNormalTexture[TERRAIN_KINDS::T_3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_N_8,
		TEXT("com_texture_n_4"), (CComponent**)&m_pNormalTexture[TERRAIN_KINDS::T_4])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::CHARACTERSELECT_TERRAIN_FILTER,
		TEXT("com_texture_Filter"), (CComponent**)&m_pFilterTexture)))
		return E_FAIL;

#pragma endregion TERRAIN_TEX

	return S_OK;
}

HRESULT CCharacterSelect_Terrain::Setup_ShaderResources()
{
	CPipeLine* pPipeline = CPipeLine::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pMainTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pDiffuseTexture[CCharacterSelect_Terrain::T_1]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_1")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CCharacterSelect_Terrain::T_2]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_2")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CCharacterSelect_Terrain::T_3]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_3")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CCharacterSelect_Terrain::T_4]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_4")))
		return E_FAIL;

	if (FAILED(m_pNormalTexture[TERRAIN_KINDS::T_1]->Setup_ShaderResource(m_pShader, "g_NormalTexture_1")))
		return E_FAIL;
	if (FAILED(m_pNormalTexture[TERRAIN_KINDS::T_2]->Setup_ShaderResource(m_pShader, "g_NormalTexture_2")))
		return E_FAIL;
	if (FAILED(m_pNormalTexture[TERRAIN_KINDS::T_3]->Setup_ShaderResource(m_pShader, "g_NormalTexture_3")))
		return E_FAIL;
	if (FAILED(m_pNormalTexture[TERRAIN_KINDS::T_4]->Setup_ShaderResource(m_pShader, "g_NormalTexture_4")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fUVSampler_Ratio_1", &m_fUVSampler_Ratio_1, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fUVSampler_Ratio_2", &m_fUVSampler_Ratio_2, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fUVSampler_Ratio_3", &m_fUVSampler_Ratio_3, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fUVSampler_Ratio_4", &m_fUVSampler_Ratio_4, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pFilterTexture->Setup_ShaderResource(m_pShader, "g_FilterTexture")))
		return E_FAIL;

	return S_OK;
}

CCharacterSelect_Terrain* CCharacterSelect_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacterSelect_Terrain* pInstance = new CCharacterSelect_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCharacterSelect_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharacterSelect_Terrain::Clone(void* pArg)
{
	CCharacterSelect_Terrain* pInstance = new CCharacterSelect_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCharacterSelect_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacterSelect_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNavigation);

	for (_uint i = 0; i < CCharacterSelect_Terrain::T_END; ++i)
	{
		Safe_Release(m_pDiffuseTexture[i]);
		Safe_Release(m_pNormalTexture[i]);
	}
	Safe_Release(m_pFilterTexture);
}

HRESULT CCharacterSelect_Terrain::Load_UVSamplerRatio_Data(const _tchar * pSamplerRatioFilePath)
{
	HANDLE		hFile = CreateFile(pSamplerRatioFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data In Terrain : UV Sampler Ratio");
		return E_FAIL;
	}

	DWORD		dwByte = { 0 };

	ReadFile(hFile, &m_fUVSampler_Ratio_1, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fUVSampler_Ratio_2, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fUVSampler_Ratio_3, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fUVSampler_Ratio_4, sizeof(_float), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
