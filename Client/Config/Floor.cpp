#include "stdafx.h"
#include "Floor.h"

#include "GameInstance.h"

CFloor::CFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CFloor::CFloor(const CFloor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFloor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CFloor::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CFloor::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CFloor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(1);
	m_pVIBuffer->Render();

	return S_OK;
}

void CFloor::RenderGUI()
{
}

HRESULT CFloor::Add_Components()
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

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-150.f, 0.f, -150.f, 1.f));

 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_FLOOR_TERRAIN,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::VTXNORTEX,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::FLOOR,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloor::Setup_ShaderResources()
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

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CFloor* CFloor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFloor* pInstance = new CFloor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFloor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFloor::Clone(void* pArg)
{
	CFloor* pInstance = new CFloor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFloor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFloor::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
}