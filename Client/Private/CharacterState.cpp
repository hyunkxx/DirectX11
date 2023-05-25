#include "stdafx.h"
#include "..\Public\CharacterState.h"

CCharacterState::CCharacterState(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacterState::CCharacterState(const CCharacterState & rhs)
	: CGameObject(rhs)
{
	memcpy(m_CharacterState, rhs.m_CharacterState, sizeof(CHARACTER_STATE) * CHARACTER_END);
}

HRESULT CCharacterState::Initialize_Prototype()
{
	// Default State
	for (int i = 0; i < CHARACTER_END; ++i)
	{
		ZeroMemory(&m_CharacterState[i], sizeof(CHARACTER_STATE));

		m_CharacterState[i].iLevel = 1;
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

HRESULT CCharacterState::Initialize(void * pArg)
{

	return S_OK;
}

void CCharacterState::Start()
{
}

void CCharacterState::PreTick(_double TimeDelta)
{
}

void CCharacterState::Tick(_double TimeDelta)
{
}

void CCharacterState::LateTick(_double TimeDelta)
{
}

void CCharacterState::RenderGUI()
{
}

void CCharacterState::AddExp(CHARACTER_SLOT eCharater, _float fExp)
{
	if (m_CharacterState[eCharater].iLevel >= MAX_LEVEL)
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

void CCharacterState::levelUp(CHARACTER_SLOT eCharater)
{
	m_CharacterState[eCharater].iLevel++;
	m_CharacterState[eCharater].fMaxExp *= 2.f;

	m_CharacterState[eCharater].fMaxHP *= 1.5f;

	m_CharacterState[eCharater].fAttack *= 1.5f;
	m_CharacterState[eCharater].fDefense *= 1.2f;
	
	m_CharacterState[eCharater].fCriticalRate += 0.5f;
	if (m_CharacterState[eCharater].fCriticalRate > 100.f)
		m_CharacterState[eCharater].fCriticalRate = 100.f;
}

CCharacterState * CCharacterState::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCharacterState* pInstance = new CCharacterState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCharacterState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCharacterState::Clone(void * pArg)
{
	CCharacterState* pInstance = new CCharacterState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCharacterState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacterState::Free()
{
	__super::Free();
}
