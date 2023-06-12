#include "stdafx.h"
#include "..\Public\Rader.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Effect.h"

CRader::CRader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice , pContext)
{

}

CRader::CRader(const CRader & rhs)
	:CGameObject(rhs)
{
}

HRESULT CRader::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	SetState(ACTIVE);

	return S_OK;
}

HRESULT CRader::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRader::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	__super::Tick(TimeDelta);
	Timer(TimeDelta);

	m_pCollider->Update(XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));

}

void CRader::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

}

HRESULT CRader::Render()
{
	return S_OK;
}

void CRader::Play_Rader(CTransform* pTransform)
{
	if (!m_bReady || nullptr == pTransform)
		return;

	m_pMainTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());
	m_WorldMatrix = pTransform->Get_WorldMatrix();

	m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"P_Rader_Effect", EFFECT_ID::COMON);
	m_pEffect->Play_Effect(&m_WorldMatrix,false);

	m_bReady = false;
	m_fCoolTime = 0.f;

}

void CRader::Timer(_double TimeDelta)
{
	if (!m_bReady)
	{
		m_fCoolTime += (_float)TimeDelta;

		if (m_fCoolTime > 5.f)
		{
			m_bReady = true;
			m_fCoolTime = 0.f;
		}
	}
}

HRESULT CRader::Add_Components()
{
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
	CollDesc.vExtents = { 1.f, 1.f , 1.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CRader * CRader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRader* pInstance = new CRader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRader::Clone(void * pArg)
{
	CRader* pInstance = new CRader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRader::Free()
{
	__super::Free();
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pCollider);
}

void CRader::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}