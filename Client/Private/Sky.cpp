#include "stdafx.h"
#include "..\Public\Sky.h"

#include "GameMode.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
	, m_eType(rhs.m_eType)
{
}

HRESULT CSky::Initialize_Prototype(SKY_TYPE eSkyType)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eType = eSkyType;

	return S_OK;
}

HRESULT CSky::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);

	m_iShaderPass_ID = { 5 };

	m_fCorrectionY = { -10.0f };

	return S_OK;
}

void CSky::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pTransformCom->Rotate(VECTOR_UP, TimeDelta);
}

void CSky::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Correction_Y_SkyPos();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCamPos));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
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

		m_pShaderCom->Begin(m_iShaderPass_ID);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CSky::Correction_Y_SkyPos()
{
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	m_vCamPos = m_vOriginCamPos = pGameInstance->Get_CamPosition();
	m_vCamPos.y = m_vOriginCamPos.y + m_fCorrectionY;
}

HRESULT CSky::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(0.3f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	// 카메라 far 범위때문에 1.0f 이상 늘리면 스카이박스 짤림. / 카메라 far 를 1000 으로 바꾸고 늘리면 됨
	m_pTransformCom->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Model */
	switch (m_eType)
	{
	case SKY_TYPE::DEFAULT:
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SKY,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	case SKY_TYPE::LOBBY:
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SKY_LOBBY,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	default:
		break;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CSky* CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, SKY_TYPE eType)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Create : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void * pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Fialed to Cloned : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
