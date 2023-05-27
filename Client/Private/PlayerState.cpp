#include "stdafx.h"
#include "..\Public\PlayerState.h"

CPlayerState::CPlayerState(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerState::CPlayerState(const CPlayerState & rhs)
	: CGameObject(rhs)
{
	memcpy(&m_PlayerState, &rhs.m_PlayerState, sizeof(PLAYER_STATE));
	memcpy(m_CharacterState, rhs.m_CharacterState, sizeof(CHARACTER_STATE) * CHARACTER_END);
	memcpy(m_CharSlot, rhs.m_CharSlot, sizeof(CHARACTERS) * SLOT_END);
}

HRESULT CPlayerState::Initialize_Prototype()
{
	// Default Player State
	ZeroMemory(&m_PlayerState, sizeof(PLAYER_STATE));
	m_PlayerState.iCharCount = 1;
	m_PlayerState.fMaxTagCooltime = 1.f;
	m_PlayerState.fCurTagCooltime = 0.f;
	m_PlayerState.fMaxQTEGauge = 100.f;
	m_PlayerState.fCurQTEGauge = 0.f;
	m_PlayerState.iCurToolID = TOOL_FIXHOOK;
	for (_uint i = 0; i < TOOL_END; ++i)
	{
		m_PlayerState.fMaxToolCooltime[i] = 5.f;
		m_PlayerState.fCurToolCooltime[i] = 0.f;
	}

	m_PlayerState.bCamLockOn = false;

	// Default Character State
	for (int i = 0; i < CHARACTER_END; ++i)
	{
		ZeroMemory(&m_CharacterState[i], sizeof(CHARACTER_STATE));

		m_CharacterState[i].iCurLevel = 1;
		m_CharacterState[i].fCurExp = 0.f;
		m_CharacterState[i].fMaxExp = 50.f;

		m_CharacterState[i].fMaxHP = 500.f;
		m_CharacterState[i].fCurHP = m_CharacterState[i].fMaxHP;

		m_CharacterState[i].fAttack[STAT_BASE] = 50.f;
		m_CharacterState[i].fDefense[STAT_BASE] = 80.f;
		m_CharacterState[i].fCriticalRate[STAT_BASE] = 5.f;
		m_CharacterState[i].fCriticalDamage[STAT_BASE] = 150.f;
		
		// ���� ĳ���� Start �ʿ��� ����
		// m_CharacterState[i].fMaxCooltime;
		// m_CharacterState[i].fMaxGauges;
	}
	lstrcpy(m_CharacterState[CHARACTER_ROVER].szName, TEXT("Rover"));
	lstrcpy(m_CharacterState[CHARACTER_YANGYANG].szName, TEXT("Yangyang"));
	lstrcpy(m_CharacterState[CHARACTER_CHIXIA].szName, TEXT("Chixia"));

	m_CharacterState[CHARACTER_ROVER].eElement = ELMT_SPECTRA;
	m_CharacterState[CHARACTER_YANGYANG].eElement = ELMT_CONDUCTO;
	m_CharacterState[CHARACTER_CHIXIA].eElement = ELMT_FUSION;

	m_CharSlot[SLOT_MAIN] = CHARACTER_ROVER;
	m_CharSlot[SLOT_SUB1] = CHARACTER_YANGYANG;
	m_CharSlot[SLOT_SUB2] = CHARACTER_CHIXIA;
	
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
	_float fTimeDelta = (_float)TimeDelta;

#pragma region CoolTime Reducing
	// �±� ��Ÿ��
	if (0.f < m_PlayerState.fCurTagCooltime)
	{
		m_PlayerState.fCurTagCooltime -= fTimeDelta;
		if (0.f > m_PlayerState.fCurTagCooltime)
			m_PlayerState.fCurTagCooltime = 0.f;
	}

	// ���� ��Ÿ��
	for (_uint i = 0; i < TOOL_END; ++i)
	{
		if (0.f < m_PlayerState.fCurToolCooltime[i])
		{
			m_PlayerState.fCurToolCooltime[i] -= fTimeDelta;
			if (0.f > m_PlayerState.fCurToolCooltime[i])
				m_PlayerState.fCurToolCooltime[i] = 0.f;
		}
	}

	// ĳ���� ��ų
	for (_uint i = 0; i < CHARACTER_END; ++i)
	{
		for (_uint j = 0; j < SKILL_END; ++j)
		{
			if (0.f < m_CharacterState[i].fCurCooltime[j])
			{
				m_CharacterState[i].fCurCooltime[j] -= fTimeDelta;
				if (0.f > m_CharacterState[i].fCurCooltime[j])
					m_CharacterState[i].fCurCooltime[j] = 0.f;
			}
		}
	}
#pragma endregion
	
#pragma region CharStat
	// ���� ȿ�� ���ӽð� üũ
	if (0.f < m_PlayerState.fFoodDuration)
	{
		m_PlayerState.fFoodDuration -= fTimeDelta;
		if (0.f > m_PlayerState.fFoodDuration)
		{
			ZeroMemory(m_PlayerState.fFoodStat, sizeof(_float) * TOOL_END);
			m_PlayerState.fFoodDuration = 0.f;
		}
			
	}
	
	// ĳ���� ���� ����س���
	for (_uint i = 0; i < CHARACTER_END; ++i)
	{
		m_CharacterState[i].fAttack[STAT_TOTAL] = (m_CharacterState[i].fAttack[STAT_BASE] + m_CharacterState[i].fAttack[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_ATK])
													* (1 + (m_CharacterState[i].fAttack[STAT_BUFF] - m_CharacterState[i].fAttack[STAT_DEBUFF]));

		m_CharacterState[i].fDefense[STAT_TOTAL] = (m_CharacterState[i].fDefense[STAT_BASE] + m_CharacterState[i].fDefense[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_DEF])
			* (1 + (m_CharacterState[i].fDefense[STAT_BUFF] - m_CharacterState[i].fDefense[STAT_DEBUFF]));

		m_CharacterState[i].fCriticalRate[STAT_TOTAL] = (m_CharacterState[i].fCriticalRate[STAT_BASE] + m_CharacterState[i].fCriticalRate[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_CRITRATE])
			* (1 + (m_CharacterState[i].fCriticalRate[STAT_BUFF] - m_CharacterState[i].fCriticalRate[STAT_DEBUFF]));

		m_CharacterState[i].fCriticalDamage[STAT_TOTAL] = (m_CharacterState[i].fCriticalDamage[STAT_BASE] + m_CharacterState[i].fCriticalDamage[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_CRITDMG])
			* (1 + (m_CharacterState[i].fCriticalDamage[STAT_BUFF] - m_CharacterState[i].fCriticalDamage[STAT_DEBUFF]));
	}
#pragma endregion
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

	m_CharacterState[eCharater].fAttack[STAT_BASE] *= 1.5f;
	m_CharacterState[eCharater].fDefense[STAT_BASE] *= 1.2f;
	
	m_CharacterState[eCharater].fCriticalRate[STAT_BASE] += 0.5f;
	if (m_CharacterState[eCharater].fCriticalRate[STAT_BASE] > 100.f)
		m_CharacterState[eCharater].fCriticalRate[STAT_BASE] = 100.f;
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
