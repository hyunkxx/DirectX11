#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Character.h"

BEGIN(Client)

class CPlayerState final : public CGameObject
{
private:
	enum { MAX_LEVEL = 50 };

public:
	enum CHARACTERS { CHARACTER_ROVER, CHARACTER_YANGYANG, CHARACTER_CHIXIA, CHARACTER_END };
	enum CHARSLOT { SLOT_MAIN, SLOT_SUB1, SLOT_SUB2, SLOT_END };

	// ���� ����
	enum STATTYPE { STYPE_ATK, STYPE_DEF, STYPE_CRITRATE, STYPE_CRITDMG, STYPE_END };
	// ���� ��ġ ����
	// BUFF/DEBUFF �� ������, ������� ���� (�������� �տ���)
	enum STAT { STAT_TOTAL, STAT_BASE, STAT_EQUIP, STAT_BUFF, STAT_DEBUFF, STAT_END };

	// ��ų ��� ���� : ��Ÿ�� / ��ġ
	enum SKILL { SKILL_Q, SKILL_E, SKILL_R, SKILL_END };

	enum TOOLS { TOOL_FIXHOOK, TOOL_LAVITATION, TOOL_SCANNER, TOOL_END};

	typedef struct tagPlayerState{
		// ���� ĳ���� �� // UI ǥ�� �� ��� ���ѿ�
		_uint		iCharCount;

		// ĳ���� ���� ��Ÿ�� / QTE �ߵ� ������
		_float		fMaxTagCooltime;
		_float		fCurTagCooltime;

		_float		fMaxQTEGauge;
		_float		fCurQTEGauge;
		
		// ���� �� ID, �� �� ��Ÿ��
		_uint		iCurToolID;
		_float		fMaxToolCooltime[TOOL_END];
		_float		fCurToolCooltime[TOOL_END];

		// ���� ȿ��
		_float		fFoodStat[STYPE_END];
		_float		fFoodDuration;

		// ķ ���� ����
		_bool		bLockOn;
		CCharacter* pLockOnTarget;


		//QTE ��� ���(Ÿ�� ���� ������ ���)
		//CCharacter* pQTETarget;
		//// ĳ���� ���� �� ���� ���
		//_float4x4	WorldMatrix;
	}PLAYER_STATE;

	// PC �⺻ ����
	typedef struct tagCharacterState
	{
		// �⺻ ���� ��ġ
		_tchar		szName[MAX_PATH];
		ELEMENT		eElement;
		_uint		iCurLevel;
		_float		fCurExp;
		_float		fMaxExp;
		
		// ���� ���� ��ġ
		_float		fCurHP;
		_float		fMaxHP;
		_float		fAttack[STAT_END];
		_float		fDefense[STAT_END];
		_float		fCriticalRate[STAT_END];
		_float		fCriticalDamage[STAT_END];

		// ��ų �� ��Ÿ�� / ��ų ������
		_float		fCurCooltime[SKILL_END];
		_float		fMaxCooltime[SKILL_END];
		_float		fCurGauge[SKILL_END];
		_float		fMaxGauge[SKILL_END];

	}CHARACTER_STATE;

private:
	CPlayerState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerState(const CPlayerState& rhs);
	virtual ~CPlayerState() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual void RenderGUI();

public: // Get
	CHARACTER_STATE* Get_CharState_byChar(_uint iCharID)
	{
		if (iCharID >= CHARACTER_END)
			return nullptr;

		return &m_CharacterState[iCharID];
	}

	CHARACTER_STATE* Get_CharState_bySlot(_uint iSlotID)
	{
		if (iSlotID >= m_PlayerState.iCharCount)
			return nullptr;

		return &m_CharacterState[m_CharSlot[iSlotID]];
	}


	_uint Get_CurToolID()
	{
		return m_PlayerState.iCurToolID;
	}

	_float Get_CurToolCoolTime()
	{
		return m_PlayerState.fCurToolCooltime[m_PlayerState.iCurToolID];
	}

	_bool Get_LockOn()
	{
		return m_PlayerState.bLockOn;
	}

	CCharacter* Get_LockOnTarget()
	{
		return m_PlayerState.pLockOnTarget;
	}
	

public: // Set
	void Set_ToolUsed(_uint iToolID)
	{
		m_PlayerState.fCurToolCooltime[iToolID] = m_PlayerState.fMaxToolCooltime[iToolID];
	}

	void Set_LockOn(_bool bLockOn, CCharacter* pTarget)
	{
		m_PlayerState.bLockOn = bLockOn;
		m_PlayerState.pLockOnTarget = pTarget;
	}


public:
	void AddExp(CHARACTERS eCharater, _float fExp);
	CHARACTER_STATE* Get_MainCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_MAIN]]); }
	CHARACTER_STATE* Get_SecondCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_SUB1]]); }
	CHARACTER_STATE* Get_ThirdCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_SUB2]]); }
	PLAYER_STATE* Get_PlayerState() { return &(m_PlayerState); }
	void AddPlayer() { if (4 > m_PlayerState.iCharCount) { ++m_PlayerState.iCharCount; } }
private:
	void levelUp(CHARACTERS eCharater);

public:
	static CPlayerState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	// ĳ���Ͱ� �ƴ� �÷��̾� ��ü�� ���� ������ ���� ����ü
	PLAYER_STATE		m_PlayerState;
	// �� ĳ���ͺ� ���� ���� ���� (��� -> ��� -> ġ��)
	CHARACTER_STATE		m_CharacterState[CHARACTER_END];
	// ĳ���� ���� ��Ȳ, enum ID���� ������� �����س���, ĳ���� ��ü�ϴ� ������ ���� ��ġ�� �ٲ�
	CHARACTERS			m_CharSlot[SLOT_END];

};

END
