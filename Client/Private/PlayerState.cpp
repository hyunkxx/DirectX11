#include "stdafx.h"
#include "..\Public\PlayerState.h"

CPlayerState::CPlayerState(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerState::CPlayerState(const CPlayerState & rhs)
	: CGameObject(rhs)
{
	memcpy(m_CharacterState, rhs.m_CharacterState, sizeof(CHARACTER_STATE) * CHARACTER_END);
}

HRESULT CPlayerState::Initialize_Prototype()
{
	// Default State
	for (int i = 0; i < CHARACTER_END; ++i)
	{
		ZeroMemory(&m_CharacterState[i], sizeof(CHARACTER_STATE));

		m_CharacterState[i].iCurLevel = 1;
		m_CharacterState[i].fCurExp = 0.f;
		m_CharacterState[i].fMaxExp = 50.f;

		m_CharacterState[i].fCurHP = 500.f;
		m_CharacterState[i].fMaxHP = 500.f;

		m_CharacterState[i].fAttack = 50.f;
		m_CharacterState[i].fDefense = 80.f;

		m_CharacterState[i].fCriticalRate = 5.f;
		m_CharacterState[i].fCriticalDamage = 150.f;
	}
	
	return S_OK;
}

HRESULT CPlayerState::Initialize(void * pArg)
{

	return S_OK;
}

void CPlayerState::Start()
{
}

void CPlayerState::PreTick(_double TimeDelta)
{
}

void CPlayerState::Tick(_double TimeDelta)
{
}

void CPlayerState::LateTick(_double TimeDelta)
{
}

void CPlayerState::RenderGUI()
{
}

void CPlayerState::AddExp(CHARACTERS eCharater, _float fExp)
{
	if (m_CharacterState[eCharater].iCurLevel >= MAX_LEVEL)
	{
		if (m_CharacterState[eCharater].fCurExp + fExp >= m_CharacterState[eCharater].fMaxExp)
			m_CharacterState[eCharater].fCurExp = m_CharacterState[eCharater].fMaxExp;

		return;
	}

	m_CharacterState[eCharater].fCurExp += fExp;

	if (m_CharacterState[eCharater].fCurExp >= m_CharacterState[eCharater].fMaxExp)
	{
		m_CharacterState[eCharater].fCurExp = (m_CharacterState[eCharater].fCurExp + fExp) - m_CharacterState[eCharater].fMaxExp;
		levelUp(eCharater);
	}

}

void CPlayerState::levelUp(CHARACTERS eCharater)
{
	m_CharacterState[eCharater].iCurLevel++;
	m_CharacterState[eCharater].fMaxExp *= 2.f;

	m_CharacterState[eCharater].fMaxHP *= 1.5f;

	m_CharacterState[eCharater].fAttack *= 1.5f;
	m_CharacterState[eCharater].fDefense *= 1.2f;
	
	m_CharacterState[eCharater].fCriticalRate += 0.5f;
	if (m_CharacterState[eCharater].fCriticalRate > 100.f)
		m_CharacterState[eCharater].fCriticalRate = 100.f;
}

CPlayerState * CPlayerState::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerState* pInstance = new CPlayerState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayerState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerState::Clone(void * pArg)
{
	CPlayerState* pInstance = new CPlayerState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState::Free()
{
	__super::Free();
}
