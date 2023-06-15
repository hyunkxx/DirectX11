#include "stdafx.h"
#include "..\Public\Boom_Rock.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Effect.h"
#include "CameraMovement.h"

CBoom_Rock::CBoom_Rock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CBoom_Rock::CBoom_Rock(const CBoom_Rock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoom_Rock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoom_Rock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_float4x4 WorldMatrix;
	memcpy(&WorldMatrix, (_float4x4*)pArg, sizeof(_float4x4));
	m_pMainTransform->Set_WorldMatrix(WorldMatrix);
	m_pMainTransform->Set_Scale(_float3(0.25f, 0.25f, 0.25f));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pMainTransform->SetRotation(vUp, XMConvertToRadians(-60.f));

	return S_OK;
}

void CBoom_Rock::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);
	
	if (m_bRimLight)
	{
		m_fRimTime += (_float)TimeDelta;

		if (m_fRimTime > 5.f)
		{
			m_fBurstRim -= (_float)TimeDelta * 0.5f;
			if (m_fBurstRim <= 0.f)
			{
				m_fBurstRim = 0.f;
				m_fRimTime = 0.f;
				m_bRimLight = false;
			}
		}
	}

	m_pCollider->Update(XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
	m_pCollider_Lader->Update(XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));

	if (m_bRender)
	{
		pGameInstance->AddCollider(m_pCollider, COLL_INTERACTION);
		pGameInstance->AddCollider(m_pCollider_Lader, COLL_RIM_INTERACTION);
	}

	if (CGameInstance::GetInstance()->InputKey(DIK_M) == KEY_STATE::TAP)
	{
		m_bRender = true;
	}
}

void CBoom_Rock::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_pRenderer != nullptr &&  m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GROUP::RENDER_DYNAMIC,this);
}

HRESULT CBoom_Rock::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResource()))
		return E_FAIL;

	m_pShader->Begin(2);
	m_pModel->Render(0);

	if (m_fBurstRim > 0.f && m_bRimLight)
	{ 
		_float vRimPower = 10.f;
		_float fAlpha = 0.5f;
		_float3 vColor = LEGEND_COLOR;
		_float4 vRimColor = _float4(vColor.x, vColor.y, vColor.z, 1.f);
		if (FAILED(m_pShader->SetRawValue("g_RimPower", &vRimPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_RimColor", &vRimColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(11);
		m_pModel->Render(0);
	}

	return S_OK;
}

HRESULT CBoom_Rock::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 17.f , 25.f , 8.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider_Lader"), (CComponent**)&m_pCollider_Lader, &CollDesc)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::MODEL,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SMODEL::INTERACTION_ROCK,
		TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoom_Rock::Setup_ShaderResource()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;
	
	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", 0, MyTextureType_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", 0, MyTextureType_NORMALS)))
		return E_FAIL;

	return S_OK;
}

CBoom_Rock * CBoom_Rock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoom_Rock* pInstance = new CBoom_Rock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBoom_Rock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoom_Rock::Clone(void * pArg)
{
	CBoom_Rock* pInstance = new CBoom_Rock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBoom_Rock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoom_Rock::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pCollider_Lader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

void CBoom_Rock::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (m_pCollider_Lader == src)
	{
		if (!m_bRimLight)
		{
			m_bRimLight = true;
			m_fRimTime = 0.f;
			m_fBurstRim = 0.3f;


		}
	}
	else if(m_pCollider == src)
	{
		m_bRender = false;
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		CGameObject* pCamera = pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement");
		static_cast<CCameraMovement*>(pCamera)->StartVibration(40.f, 1.f);
		// 여기서 몬스터 소환 및 사운드 변경
	}
}
