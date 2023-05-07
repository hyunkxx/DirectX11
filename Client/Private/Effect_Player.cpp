#include "stdafx.h"
#include "..\Public\Effect_Player.h"

#include "GameInstance.h"

#include "Mesh_Effect_P.h"
#include "Particle_Effect_P.h"
#include "Rect_Effect_P.h"

CEffect_Player::CEffect_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice , pContext)
{

}

CEffect_Player::CEffect_Player(const CEffect_Player & rhs)
	:CEffect(rhs)
{

}

HRESULT CEffect_Player::Initialize_Prototype(const char* FilePath , const list<EFFECT_DESC*>& pEffectDescList)
{

	for (auto& iter : pEffectDescList)
	{
		CEffect* pEffect = nullptr;
		if (EFFECT_TYPE::ID_MESH == iter->eEffectType)
		{
			pEffect = CMesh_Effect_P::Create(m_pDevice, m_pContext, FilePath,*iter);
		}
		else if (EFFECT_TYPE::ID_PARTICLE == iter->eEffectType)
		{
			pEffect = CParticle_Effect_P::Create(m_pDevice, m_pContext, FilePath,*iter);
		}
		else if (EFFECT_TYPE::TYPE_RECT ==  iter->eEffectType)
		{
			pEffect = CRect_Effect_P::Create(m_pDevice, m_pContext, FilePath, *iter);
		}
		if (nullptr != pEffect)
		{
			m_EffectList.push_back(pEffect);
		}
	}

	return S_OK;
}

HRESULT CEffect_Player::Initialize(void * pArg)
{
	return S_OK;
}

void CEffect_Player::Tick(_double TimeDelta)
{
	if (m_bFinish)
		return;

	for (auto& iter : m_EffectList)
	{
		iter->Tick(TimeDelta);
	}
}

void CEffect_Player::LateTick(_double TimeDelta)
{
	if (m_bFinish)
		return;

	_bool bFinish = true;
	for (auto& iter : m_EffectList)
	{
		iter->LateTick(TimeDelta);
		if (!iter->Get_Finish())
			bFinish = false;
	}

	if (bFinish)
	{
		m_bFinish = true;
		m_bEffectUpdate = false;
	}
}

HRESULT CEffect_Player::Render()
{
	return S_OK;
}

void CEffect_Player::RenderGUI()
{

}

void CEffect_Player::Play_Effect(_float4x4* pWorldMatrix, _bool bTracking)
{
	m_EffectDesc.bTracking = bTracking;
	m_bFinish = false;
	m_pParentsMatrix = nullptr;

	if(m_EffectDesc.bTracking)
	{
		m_pParentsMatrix = pWorldMatrix;
		for (auto& iter : m_EffectList)
		{
			iter->Play_Effect(m_pParentsMatrix);
		}
	}
	else
	{
		m_pParentsMatrix = nullptr;
		memcpy(&m_ParentsMatrix, pWorldMatrix, sizeof(_float4x4));
		for (auto& iter : m_EffectList)
		{
			iter->Play_Effect(&m_ParentsMatrix);
		}
	}
}

list<EFFECT_DESC*>* CEffect_Player::Get_Effects()
{
	list<EFFECT_DESC*>* pEffects = new list<EFFECT_DESC*>;

	for (auto&iter : m_EffectList)
	{
		EFFECT_DESC* pEffectDesc = new EFFECT_DESC;
		memcpy(pEffectDesc, &iter->Get_Effect_Desc(),sizeof(EFFECT_DESC));

		pEffects->push_back(pEffectDesc);
	}

	return pEffects;
}

_float CEffect_Player::Get_LifeTime()
{
	_float fLifeTime = 0.f;

	for (auto& iter : m_EffectList)
	{
		if (fLifeTime < iter->Get_Effect_Desc().fLifeTime)
		{
			fLifeTime = iter->Get_Effect_Desc().fLifeTime;
		}
	}

	return fLifeTime;
}


void CEffect_Player::Add_Effect(CEffect * pEffect)
{
	if (nullptr == pEffect)
		return;

	m_EffectList.push_back(pEffect);

}

CEffect_Player * CEffect_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* FilePath , const list<EFFECT_DESC*>& pEffectDescList)
{
	CEffect_Player*		pInstance = new CEffect_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(FilePath , pEffectDescList)))
	{
		MSG_BOX("Failed to Created : CEffect_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Player::Clone(void * pArg)
{
	CGameObject*		pInstance = new CEffect_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Player::Free()
{
	__super::Free();

	for (auto& iter : m_EffectList)
	{
		Safe_Release(iter);
	}
	m_EffectList.clear();
}
