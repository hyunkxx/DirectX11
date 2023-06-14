#include "stdafx.h"
#include "..\Public\Object_Bomer.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Character.h"
#include "Effect.h"
#include "PlayerState.h"

CObject_Bomber::CObject_Bomber(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CObject_Bomber::CObject_Bomber(const CObject_Bomber & rhs)
	: CGameObject(rhs)
{
}

HRESULT CObject_Bomber::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Bomber::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_matrix IdenMat = XMMatrixIdentity();
	XMStoreFloat4x4(&m_CatchMatrix, IdenMat);
	m_CatchMatrix._41 = 0.5f;
	m_CatchMatrix._42 = 1.5f;
	m_CatchMatrix._43 = 0.5f;

	memcpy(&m_StartMatrix, (_float4x4*)pArg, sizeof(_float4x4));
	memcpy(&m_WorldMatrix, (_float4x4*)pArg, sizeof(_float4x4));
	m_vStartPos = _float3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);

	CGameInstance * pGameInstance =CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));

	if (FAILED(Add_Components()))
		return E_FAIL;

	SetState(ACTIVE);

	return S_OK;
}

void CObject_Bomber::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	State_Control(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider, COLL_INTERACTION);
}

void CObject_Bomber::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CObject_Bomber::Render()
{
	return S_OK;
}

void CObject_Bomber::State_Control(_double TimeDelta)
{
	_float3 vPosition , vLook , LinearPosition;
	_vector vStartPosition, EndPosition;
	_float4x4 FinishMatrix;
	CEffect*  pEffect = nullptr;
	_float fDist = 10.f; 

	switch (m_eState)
	{
	case Client::CObject_Bomber::ID_IDLE:
		m_pTarget = m_pPlayerState->Get_ActiveCharacter();
		XMStoreFloat3(&vPosition, m_pTarget->GetTransform()->Get_State(CTransform::STATE::STATE_POSITION));
		fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vStartPos) - XMLoadFloat3(&vPosition)));

		if (fDist < 8.f)
		{
			if(CGameInstance::GetInstance()->InputKey(DIK_F) == KEY_STATE::TAP)
				Catch_Bomber(m_pTarget->GetTransform());
		}
		break;

	case Client::CObject_Bomber::ID_CATCH:
		m_fCatchTime += (_float)TimeDelta;
		if (m_fCatchTime <= 1.f)
		{
			vStartPosition = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
			XMStoreFloat4x4(&FinishMatrix, XMLoadFloat4x4(&m_CatchMatrix) * XMLoadFloat4x4(&m_pCatchTransform->Get_WorldMatrix()));
			EndPosition = XMVectorSet(FinishMatrix._41, FinishMatrix._42, FinishMatrix._43, 1.f);
			XMStoreFloat3(&LinearPosition, XMVectorLerp(vStartPosition, EndPosition, m_fCatchTime));

			m_WorldMatrix._41 = LinearPosition.x;
			m_WorldMatrix._42 = LinearPosition.y;
			m_WorldMatrix._43 = LinearPosition.z;
		}
		else
		{
			XMStoreFloat4x4(&m_WorldMatrix  , XMLoadFloat4x4(&m_CatchMatrix) * XMLoadFloat4x4(&m_pCatchTransform->Get_WorldMatrix()));
			Shoot_Bomber();
		}
		break;

	case Client::CObject_Bomber::ID_THROW:
		m_fThrowTime += (_float)TimeDelta;

		if (m_fThrowTime > 3.f)
		{
			m_fThrowTime = 0.f;
			m_eState = ID_IDLE;
			m_WorldMatrix = m_StartMatrix;
		}

		vLook = _float3(m_vThrowLook.x * m_fSpeed * (_float)TimeDelta, m_vThrowLook.y * m_fSpeed * (_float)TimeDelta, m_vThrowLook.z * m_fSpeed * (_float)TimeDelta);
		m_WorldMatrix._41 += vLook.x;
		m_WorldMatrix._42 += vLook.y;
		m_WorldMatrix._43 += vLook.z;
		break;

	case Client::CObject_Bomber::ID_BOOM:
		m_fResetTime += (_float)TimeDelta;
		if (nullptr != m_pEffect)
		{
			m_pEffect->Shut_Down();
			m_pEffect = nullptr;

			pEffect = CGameInstance::GetInstance()->Get_Effect(L"Object_Boom_Effect", EFFECT_ID::COMON);
			if (nullptr == pEffect)
				return;
			pEffect->Play_Effect(&m_WorldMatrix, false);
		}
		if (m_fResetTime > 5.f)
		{
			m_fResetTime = 0.f;
			memcpy(&m_WorldMatrix, &m_StartMatrix, sizeof(_float4x4));
			m_eState = ID_IDLE;
			m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"Object_Bomber", EFFECT_ID::COMON);
			if (nullptr == m_pEffect)
				return;
			m_pEffect->Play_Effect(&m_WorldMatrix, true);
		}
		break;

	case Client::CObject_Bomber::STATE_END:
		break;

	default:
		break;
	}

	m_pCollider->Update(XMLoadFloat4x4(&m_WorldMatrix));

}

void CObject_Bomber::Catch_Bomber(CTransform * pTransform)
{
	if (nullptr == pTransform)
		return;

	m_pCatchTransform = pTransform;
	m_eState = ID_CATCH;
	m_fCatchTime = 0.f;
}

void CObject_Bomber::Shoot_Bomber()
{
	if (m_eState != ID_CATCH)
		return;

	if (CGameInstance::GetInstance()->InputMouse(DIMK_LB) == KEY_STATE::TAP)
	{
		XMStoreFloat3(&m_vThrowLook, XMVector3Normalize(m_pCatchTransform->Get_State(CTransform::STATE::STATE_LOOK)));
		m_fThrowTime = 0.f;
		m_eState = ID_THROW;

		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Object_Bomber_Effect_02", EFFECT_ID::COMON);
		if (nullptr == pEffect)
			return;
		pEffect->Play_Effect(&m_WorldMatrix, true);
	}
}

HRESULT CObject_Bomber::Add_Components()
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
		TEXT("Com_Collider_Boom"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"Object_Bomber",EFFECT_ID::COMON);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Play_Effect(&m_WorldMatrix,true);

	return S_OK;
}

CObject_Bomber * CObject_Bomber::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObject_Bomber* pInstance = new CObject_Bomber(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CObject_Bomber");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CObject_Bomber::Clone(void * pArg)
{
	CObject_Bomber* pInstance = new CObject_Bomber(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CObject_Bomber");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Bomber::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pCollider);
}

void CObject_Bomber::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (m_eState != ID_THROW)
		return;

	m_eState = ID_BOOM;
	m_fResetTime = 0.f;
}
