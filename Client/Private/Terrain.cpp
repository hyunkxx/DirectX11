#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iShader_PassID = { 0 };

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_U))
	{
		if (0 == m_iShader_PassID)
			m_iShader_PassID = 2;
		else if (2 == m_iShader_PassID)
			m_iShader_PassID = 0;
	}
}

void CTerrain::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);

#ifdef _DEBUG
	// 디버그 모드 랜더링 안할 시 릭 나옴
	/*if (nullptr != m_pRenderer && nullptr != m_pNavigation)
	{
		m_pNavigation->Set_IntervalY(1.0f);
		m_pRenderer->AddDebugGroup(m_pNavigation);
	}*/
#endif // _DEBUG
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(m_iShader_PassID);
	m_pVIBuffer->Render();

	return S_OK;   
}

void CTerrain::RenderGUI()
{
	ImGui::Begin("Terrain ID");
	ImGui::DragInt("ID", (int*)&m_iObjectID);
	ImGui::End();

	return;
}

HRESULT CTerrain::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_TERRAIN,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::VTXNORTEX,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::NAVIGATION,
		TEXT("com_navigation"), (CComponent**)&m_pNavigation)))
		return E_FAIL;

#pragma region TERRAIN_TEX
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_1,
		TEXT("com_texture_diffuse_1"), (CComponent**)&m_pDiffuseTexture[T_1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_2,
		TEXT("com_texture_diffuse_2"), (CComponent**)&m_pDiffuseTexture[T_2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_3,
		TEXT("com_texture_diffuse_3"), (CComponent**)&m_pDiffuseTexture[T_3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_D_4,
		TEXT("com_texture_diffuse_4"), (CComponent**)&m_pDiffuseTexture[T_4])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::TERRAIN_FILTER,
		TEXT("com_texture_Filter"), (CComponent**)&m_pFilterTexture)))
		return E_FAIL;
	
#pragma endregion TERRAIN_TEX

	return S_OK;
}

HRESULT CTerrain::Setup_ShaderResources()
{
	CPipeLine* pPipeline= CPipeLine::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pMainTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pDiffuseTexture[CTerrain::T_1]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_1")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CTerrain::T_2]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_2")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CTerrain::T_3]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_3")))
		return E_FAIL;
	if (FAILED(m_pDiffuseTexture[CTerrain::T_4]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture_4")))
		return E_FAIL;

	if (FAILED(m_pFilterTexture->Setup_ShaderResource(m_pShader, "g_FilterTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNavigation);

	for (_uint i = 0; i < CTerrain::T_END; ++i)
		Safe_Release(m_pDiffuseTexture[i]);

	Safe_Release(m_pFilterTexture);
}