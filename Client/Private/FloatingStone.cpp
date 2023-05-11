#include "stdafx.h"
#include "..\Public\FloatingStone.h"

#include "GameMode.h"
#include "GameInstance.h"

CFloatingStone::CFloatingStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFloatingStone::CFloatingStone(const CFloatingStone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFloatingStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloatingStone::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_float fRandX = _float(rand() % 40 - 20);
	_float fRandY = _float(rand() % 3 + 1);
	_float fRandZ = _float(rand() % 40 - 20);
	_float fRandAngle = _float(rand() % 360);

	int iRandPer = rand() % 10 + 2;
	int iRandScale = rand() % 2;

	_float3 vScale = _float3(iRandScale + (iRandPer * 0.1f), iRandScale + (iRandPer * 0.1f), iRandScale + (iRandPer * 0.1f));

	_vector vPos = XMVectorSet(fRandX, fRandY, fRandZ, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pMainTransform->Set_Scale(vScale);
	m_pMainTransform->SetRotation(VECTOR_LOOK, XMConvertToRadians(fRandAngle));

	m_fStartTimeLimit = fRandY + iRandPer * 0.1f;

	return S_OK;
}

void CFloatingStone::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fStartTimeAcc += (_float)TimeDelta;
	if (m_fStartTimeAcc >= m_fStartTimeAcc)
		floatingPosition(TimeDelta);
}

void CFloatingStone::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
}

HRESULT CFloatingStone::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pModel->Render(0);

	return S_OK;
}

void CFloatingStone::RenderGUI()
{
}

HRESULT CFloatingStone::Add_Components()
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
	
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_FLOATING_ROCK_01,
		TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloatingStone::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pMainTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", 0, MyTextureType_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", 0, MyTextureType_NORMALS)))
		return E_FAIL;

	return S_OK;
}

void CFloatingStone::floatingPosition(_double TimeDelta)
{
	_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);

	m_fTimeAcc += (_float)TimeDelta;
	_float fHeight = sinf(m_fTimeAcc) * 0.1f;

	vCurPos = vCurPos + VECTOR_UP * fHeight * (_float)TimeDelta;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
}

CFloatingStone * CFloatingStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFloatingStone* pInstance = new CFloatingStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFloatingStone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFloatingStone::Clone(void * pArg)
{
	CFloatingStone* pInstance = new CFloatingStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFloatingStone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFloatingStone::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}