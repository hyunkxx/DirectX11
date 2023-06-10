#include "stdafx.h"
#include "..\Public\PlayerState.h"
#include "ItemDB.h"

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

	m_PlayerState.bLockOn = false;

	// Default Character State
	for (int i = 0; i < CHARACTER_END; ++i)
	{
		ZeroMemory(&m_CharacterState[i], sizeof(CHARACTER_STATE));

		m_CharacterState[i].iCurLevel = 1;
		m_CharacterState[i].fCurExp = 0.f;
		m_CharacterState[i].fMaxExp = 500.f;

		m_CharacterState[i].fMaxHP = 500.f;
		m_CharacterState[i].fCurHP = m_CharacterState[i].fMaxHP;

		m_CharacterState[i].fAttack[STAT_BASE] = 50.f;
		m_CharacterState[i].fDefense[STAT_BASE] = 80.f;
		m_CharacterState[i].fCriticalRate[STAT_BASE] = 5.f;
		m_CharacterState[i].fCriticalDamage[STAT_BASE] = 150.f;
		
		// 개별 캐릭터 Start 쪽에서 설정
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
	CItemDB* pDB = CItemDB::GetInstance();
	m_EquipWeapons[CHARACTER_ROVER] = pDB->GetItemData(ITEM::SWORD0);
	m_EquipWeapons[CHARACTER_YANGYANG] = pDB->GetItemData(ITEM::SWORD0);
	m_EquipWeapons[CHARACTER_CHIXIA] = pDB->GetItemData(ITEM::GUN0);
	
	ZeroMemory(m_fSequenceValue, sizeof m_fSequenceValue);
	ZeroMemory(m_AttackDesc, sizeof m_AttackDesc);

	m_AttackDesc[CHARACTER_ROVER].fDamageFactor[0] = 1.f;
	m_AttackDesc[CHARACTER_ROVER].fDamageFactor[1] = 1.f;
	m_AttackDesc[CHARACTER_ROVER].fDamageFactor[2] = 1.f;
	m_AttackDesc[CHARACTER_ROVER].fDamageFactor[3] = 1.f;
	m_AttackDesc[CHARACTER_ROVER].fBurstFactor = 1.f;
	m_AttackDesc[CHARACTER_ROVER].bAirboneQTE = false;

	m_AttackDesc[CHARACTER_YANGYANG].fDamageFactor[0] = 1.f;
	m_AttackDesc[CHARACTER_YANGYANG].fDamageFactor[1] = 1.f;
	m_AttackDesc[CHARACTER_YANGYANG].fDamageFactor[2] = 1.f;
	m_AttackDesc[CHARACTER_YANGYANG].fDamageFactor[3] = 1.f;
	m_AttackDesc[CHARACTER_YANGYANG].fBurstFactor = 1.f;
	m_AttackDesc[CHARACTER_YANGYANG].bAirboneQTE = false;

	m_AttackDesc[CHARACTER_CHIXIA].fDamageFactor[0] = 1.f;
	m_AttackDesc[CHARACTER_CHIXIA].fDamageFactor[1] = 1.f;
	m_AttackDesc[CHARACTER_CHIXIA].fDamageFactor[2] = 1.f;
	m_AttackDesc[CHARACTER_CHIXIA].fDamageFactor[3] = 1.f;
	m_AttackDesc[CHARACTER_CHIXIA].fBurstFactor = 1.f;
	m_AttackDesc[CHARACTER_CHIXIA].bAirboneQTE = false;

	return S_OK;
}

void CPlayerState::Start()
{
}

void CPlayerState::PreTick(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;

#pragma region CoolTime Reducing
	// 태그 쿨타임
	if (0.f < m_PlayerState.fCurTagCooltime)
	{
		m_PlayerState.fCurTagCooltime -= fTimeDelta;
		if (0.f > m_PlayerState.fCurTagCooltime)
			m_PlayerState.fCurTagCooltime = 0.f;
	}

	// 교대 쿨타임
	for (_uint i = 0; i < TOOL_END; ++i)
	{
		if (0.f < m_PlayerState.fCurToolCooltime[i])
		{
			m_PlayerState.fCurToolCooltime[i] -= fTimeDelta;
			if (0.f > m_PlayerState.fCurToolCooltime[i])
				m_PlayerState.fCurToolCooltime[i] = 0.f;
		}
	}

	// 캐릭터 스킬
	for (_uint i = 0; i < CHARACTER_END; ++i)
	{
		for (_uint j = 0; j < COOL_END; ++j)
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
	// 음식 효과 지속시간 체크
	if (0.f < m_PlayerState.fFoodDuration)
	{
		m_PlayerState.fFoodDuration -= fTimeDelta;
		if (0.f > m_PlayerState.fFoodDuration)
		{
			ZeroMemory(m_PlayerState.fFoodStat, sizeof(_float) * TOOL_END);
			m_PlayerState.fFoodDuration = 0.f;
		}
			
	}
	
	// 캐릭터 스탯 계산해놓기 ( 시퀀스체인, 무기착용 추가 )
	for (_uint i = 0; i < CHARACTER_END; ++i)
	{
		_float fAttack = m_CharacterState[i].fAttack[STAT_BASE];
		_float fCriRate = m_CharacterState[i].fCriticalRate[STAT_BASE];
		_float fDefense = m_CharacterState[i].fDefense[STAT_BASE];
		
		fAttack += m_CharacterState[i].fAttack[STAT_BASE] * m_fSequenceValue[SQUENCE1];
		fCriRate += m_fSequenceValue[SQUENCE2];
		fDefense += m_CharacterState[i].fDefense[STAT_BASE] * m_fSequenceValue[SQUENCE3];

		m_CharacterState[i].fAttack[STAT_EQUIP] = (_float)CItemDB::ComputeWeaponDamage(m_EquipWeapons[i]);
		m_CharacterState[i].fAttack[STAT_TOTAL] = (fAttack + m_CharacterState[i].fAttack[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_ATK])
													* (1 + (m_CharacterState[i].fAttack[STAT_BUFF] - m_CharacterState[i].fAttack[STAT_DEBUFF]));

		m_CharacterState[i].fDefense[STAT_TOTAL] = (fDefense + m_CharacterState[i].fDefense[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_DEF])
			* (1 + (m_CharacterState[i].fDefense[STAT_BUFF] - m_CharacterState[i].fDefense[STAT_DEBUFF]));
		
		m_CharacterState[i].fCriticalRate[STAT_EQUIP] = (_float)CItemDB::ComputeWeaponCriticalRate(m_EquipWeapons[i]);
		m_CharacterState[i].fCriticalRate[STAT_TOTAL] = (fCriRate + m_CharacterState[i].fCriticalRate[STAT_EQUIP] + m_PlayerState.fFoodStat[STYPE_CRITRATE])
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

void CPlayerState::SetEquitWeapon(CHARACTERS eCharType, CItem::ITEM_DESC eWeapon)
{
	assert(eWeapon.eItemType == CItem::ITEN_WEAPON);
	m_EquipWeapons[eCharType] = eWeapon;
}

void CPlayerState::AddExp(CHARACTERS eCharater, _float fExp)
{
	if (m_CharacterState[eCharater].iCurLevel >= MAX_LEVEL)
	{
		if (m_CharacterState[eCharater].fCurExp + fExp >= m_CharacterState[eCharater].fMaxExp)
			m_CharacterState[eCharater].fCurExp = m_CharacterState[eCharater].fMaxExp;

		return;
	}

	_float fTotalExp = fExp;

	while (fTotalExp > 0.f)
	{
		if (m_CharacterState[eCharater].fMaxExp < fTotalExp)
		{
			fTotalExp -= m_CharacterState[eCharater].fMaxExp - m_CharacterState[eCharater].fCurExp;
			levelUp(eCharater);
			m_CharacterState[eCharater].fCurExp = 0.f;

			continue;
		}
		else
		{
			m_CharacterState[eCharater].fCurExp += fTotalExp;

			if (m_CharacterState[eCharater].fCurExp >= m_CharacterState[eCharater].fMaxExp)
			{
				m_CharacterState[eCharater].fCurExp = (m_CharacterState[eCharater].fCurExp + fTotalExp) - m_CharacterState[eCharater].fMaxExp;
				levelUp(eCharater);
			}

			fTotalExp = 0.f;
		}
	}

}

void CPlayerState::SetSequenceLevel(SQ_TYPE eType, _uint iGrade)
{
	switch (eType)
	{
	case CPlayerState::SQUENCE1:
		if (iGrade == 1)
			m_fSequenceValue[SQUENCE1] = 0.2f;
		else if (iGrade == 2)
			m_fSequenceValue[SQUENCE1] = 0.3f;
		else if (iGrade == 3)
			m_fSequenceValue[SQUENCE1] = 0.5f;

		break;
	case CPlayerState::SQUENCE2:
		if (iGrade == 1)
			m_fSequenceValue[SQUENCE2] = 2.f;
		else if (iGrade == 2)
			m_fSequenceValue[SQUENCE2] = 3.f;
		else if (iGrade == 3)
			m_fSequenceValue[SQUENCE2] = 5.f;

		break;
	case CPlayerState::SQUENCE3:
		if (iGrade == 1)
			m_fSequenceValue[SQUENCE3] = 0.2f;
		else if (iGrade == 2)
			m_fSequenceValue[SQUENCE3] = 0.3f;
		else if (iGrade == 3)
			m_fSequenceValue[SQUENCE3] = 0.5f;

		break;
	case CPlayerState::SQUENCE4:
		if (iGrade == 1)
			m_fSequenceValue[SQUENCE4] = 0.2f;
		else if (iGrade == 2)
			m_fSequenceValue[SQUENCE4] = 0.3f;
		else if (iGrade == 3)
			m_fSequenceValue[SQUENCE4] = 0.5f;

		break;
	default:
		break;
	}

	for (_uint i = 0; i < 3; ++i)
		m_CharacterState[i].fMaxHP += m_CharacterState[i].fMaxHP * m_fSequenceValue[SQUENCE4];
}

void CPlayerState::SetAttackFactor(CHARACTERS eCharacter, _uint iAttack, _float fFactor)
{
	assert(eCharacter < 3);
	assert(iAttack < 4);

	m_AttackDesc[eCharacter].fDamageFactor[iAttack] = fFactor;
	m_iSkillPoint[eCharacter]--;
}

void CPlayerState::SeBurstFactor(CHARACTERS eCharacter, _float fFactor)
{
	assert(eCharacter < 3);
	m_AttackDesc[eCharacter].fBurstFactor = fFactor;
	m_iSkillPoint[eCharacter]--;
}

void CPlayerState::SetAirboneQTE(CHARACTERS eCharacter, _bool bAirbone)
{
	assert(eCharacter < 3);
	m_AttackDesc[eCharacter].bAirboneQTE = bAirbone;
	m_iSkillPoint[eCharacter]--;
}

void CPlayerState::levelUp(CHARACTERS eCharater)
{
	if (m_CharacterState[eCharater].iCurLevel % 4 == 0)
		m_iSkillPoint[eCharater]++;

	m_CharacterState[eCharater].iCurLevel++;
	m_CharacterState[eCharater].fMaxExp *= 1.1f;

	m_CharacterState[eCharater].fMaxHP *= 1.1f;

	m_CharacterState[eCharater].fAttack[STAT_BASE] += 2.5f;
	m_CharacterState[eCharater].fDefense[STAT_BASE] += 1.5f;
	
	m_CharacterState[eCharater].fCriticalRate[STAT_BASE] += 0.25f;
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
