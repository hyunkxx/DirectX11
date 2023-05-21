#include "stdafx.h"
#include "..\Public\Missile.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Effect.h"
#include "Character.h"

CMissile::CMissile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMissile::CMissile(const CMissile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMissile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_tMissileDesc, pArg, sizeof(MISSILEDESC));
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	SetState(DISABLE);

	return S_OK;
}

void CMissile::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	m_LifeTimeTimer -= TimeDelta;

	if (0.0 > m_LifeTimeTimer)
	{
		m_pCollider->SetActive(false);
		SetState(DISABLE);
	}
	

	if (0.0 != m_tMissileDesc.HitInterval)
	{
		m_HitIntervalTimer -= TimeDelta;

		if (0.0 > m_HitIntervalTimer)
			m_pCollider->SetActive(true);
		else
			m_pCollider->SetActive(false);
	}
	

	if(CCharacter::CT_PLAYER == m_tMissileDesc.pOwner->Get_CollType())
		pGameInstance->AddCollider(m_pCollider, COLL_PLAYERATTACK);
	else if (CCharacter::CT_MONSTER == m_tMissileDesc.pOwner->Get_CollType())
		pGameInstance->AddCollider(m_pCollider, COLL_MONSTERATTACK);

	m_pCollider->Update(XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
}

void CMissile::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
}

HRESULT CMissile::Render()
{
	__super::Render();

#ifdef _DEBUG
	m_pCollider->Render();
#endif
	return S_OK;

}

HRESULT CMissile::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;
	
	return S_OK;
}

_bool CMissile::Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix)
{
	if (ACTIVE == m_eState)
		return false;

	SetState(ACTIVE);
	m_HitIntervalTimer = 0.0;
	m_LifeTimeTimer = m_tMissileDesc.LifeTime;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vInitPos);
	m_pMainTransform->Set_LookDir(XMVector3TransformNormal(vLookDir, vMissileRotMatrix));

	if(lstrcmp(m_tMissileDesc.szEffectTag, TEXT("")))
		CGameInstance::GetInstance()->Get_Effect(m_tMissileDesc.szEffectTag, (Engine::EFFECT_ID)m_tMissileDesc.iEffectLayer)->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), true);

	m_pCollider->SetActive(true);

	m_pCollider->Update(XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
	
	return true; 
}

_vector CMissile::Get_Position()
{
	return m_pMainTransform->Get_State(CTransform::STATE_POSITION);
}

void CMissile::OnCollisionOnTarget(CCollider * src, CCollider * dest)
{
	// 콜라이더 사이 중점 찾아서 히트 이펙트 출력

	// 카메라 쉐이크 처리 등

	// HitInterval 처리
	if (0.0 != m_tMissileDesc.HitInterval)
	{
		m_pCollider->SetActive(false);
		m_HitIntervalTimer = m_tMissileDesc.HitInterval;
	}
}

HRESULT CMissile::Add_Components()
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

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { m_tMissileDesc.fExtents, m_tMissileDesc.fExtents, m_tMissileDesc.fExtents };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CMissile * CMissile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissile* pInstance = new CMissile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMissile::Clone(void * pArg)
{
	CMissile* pInstance = new CMissile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile::Free()
{
	__super::Free();
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pCollider);
}

void CMissile::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
		if (CCharacter::CT_PLAYER == m_tMissileDesc.pOwner->Get_CollType())
		{
			if (CCharacter::CT_MONSTER == pOpponent->Get_CollType())
			{
				if (true == dest->Compare(pOpponent->GetHitCollider()))
					OnCollisionOnTarget(src, dest);
			}
		}
		else if (CCharacter::CT_MONSTER == m_tMissileDesc.pOwner->Get_CollType())
		{
			if (CCharacter::CT_PLAYER == pOpponent->Get_CollType())
			{
				if (true == dest->Compare(pOpponent->GetHitCollider()))
					OnCollisionOnTarget(src, dest);
			}
		}
	}
}

void CMissile::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CMissile::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
