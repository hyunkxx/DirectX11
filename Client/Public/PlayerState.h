#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Character.h"
#include "ItemDB.h"

BEGIN(Client)

class CPlayerState final : public CGameObject
{
private:
	enum { MAX_LEVEL = 50 };

public:
	enum CHARACTERS { CHARACTER_ROVER, CHARACTER_YANGYANG, CHARACTER_CHIXIA, CHARACTER_END };
	enum CHARSLOT { SLOT_MAIN, SLOT_SUB1, SLOT_SUB2, SLOT_END };
	enum TOOLS { TOOL_FIXHOOK, TOOL_LAVITATION, TOOL_SCANNER, TOOL_END };

	// ���� ����
	enum STATTYPE { STYPE_ATK, STYPE_DEF, STYPE_CRITRATE, STYPE_CRITDMG, STYPE_END };
	// ���� ��ġ ����
	// BUFF/DEBUFF �� ������, ������� ���� (�������� �տ���)
	enum STAT { STAT_TOTAL, STAT_BASE, STAT_EQUIP, STAT_BUFF, STAT_DEBUFF, STAT_END };

	//  ��Ÿ��
	enum CHARCOOLTIMES { COOL_ECHO, COOL_SKILL, COOL_BURST, COOL_END };

	// ������
	enum CHARGAUGES { GAUGE_SPECIAL, GAUGE_BURST, GAUGE_END };

	//����
	enum FOOD_BUFF { FOOD_DEFENSE, FOOD_ATTACK, FOOD_CRIRATE, FOOD_END };

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

		// chixia ���� ���� ���� ����� ����
		_bool		bAiming;

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
		_float		fCurCooltime[COOL_END];
		_float		fMaxCooltime[COOL_END];
		_float		fCurGauge[GAUGE_END];
		_float		fMaxGauge[GAUGE_END];

	}CHARACTER_STATE;

	typedef struct tagAttackDesc
	{
		_float fDamageFactor[4];	//�⺻���� 1Ÿ~4Ÿ ���
		_float fBurstFactor;		//�ñر� ���
		_bool  bAirboneQTE;			//QTE Big->Airbone

	}ATTACK_DESC;

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

	// �÷��̾� ĳ���͸� ����Ѵ�.
	void Register_Character(CPlayerState::CHARACTERS eCharacterID, CCharacter* pCharacter, _bool* bOnControl)
	{
		m_pCharacter[eCharacterID] = pCharacter;

		if (m_CharSlot[SLOT_MAIN] == eCharacterID)
			*bOnControl = true;
		else
			*bOnControl = false;
	}

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

	_bool Get_Aiming()
	{
		return m_PlayerState.bAiming;
	}

	CCharacter* Get_LockOnTarget()
	{
		return m_PlayerState.pLockOnTarget;
	}

	void Gain_QTEGauge(_float fQTEGauge)
	{
		m_PlayerState.fCurQTEGauge += fQTEGauge;
		if (m_PlayerState.fCurQTEGauge > m_PlayerState.fMaxQTEGauge)
			m_PlayerState.fCurQTEGauge = m_PlayerState.fMaxQTEGauge;
	}

	_uint Get_Slot(_uint iCharacterID)
	{
		if (m_CharSlot[SLOT_MAIN] == iCharacterID)
			return SLOT_MAIN;
		else if (m_CharSlot[SLOT_SUB1] == iCharacterID)
			return SLOT_SUB1;
		else 
			return SLOT_SUB2;
	}
	
	CItem::ITEM_DESC* GetCurWeaponDesc(CHARACTERS eCharType) { return &m_EquipWeapons[eCharType]; }
	void SetEquitWeapon(CHARACTERS eCharType, CItem::ITEM_DESC eWeapon);

	CCharacter* Get_ActiveCharacter()
	{
		return m_pCharacter[m_CharSlot[SLOT_MAIN]];
	}

	_bool Change_ActiveCharacter(_uint iSubID); 

public: // Set
	void Set_ToolUsed()
	{
		m_PlayerState.fCurToolCooltime[m_PlayerState.iCurToolID] = m_PlayerState.fMaxToolCooltime[m_PlayerState.iCurToolID];
	}

	void Set_LockOn(_bool bLockOn, CCharacter* pTarget)
	{
		m_PlayerState.bLockOn = bLockOn;
		m_PlayerState.pLockOnTarget = pTarget;
	}

	void Set_Aiming(_bool bAiming)
	{
		m_PlayerState.bAiming = bAiming;
	}

	void Set_ActiveCharacter(_uint eCharacterID)
	{
		if (CHARACTER_END <= eCharacterID || eCharacterID == m_CharSlot[SLOT_MAIN])
			return;

		CHARACTERS eTemp = m_CharSlot[SLOT_MAIN];
		m_CharSlot[SLOT_MAIN] = CHARACTERS(eCharacterID);

		if (m_CharSlot[SLOT_SUB1] == eCharacterID)
			m_CharSlot[SLOT_SUB1] = eTemp;
		else 
			m_CharSlot[SLOT_SUB2] = eTemp;
	}

	void Set_PlayerCamera(class CPlayerCamera* pPCam) 
	{ 
		m_pPlayerCam = pPCam; 
	}


public:
	void SetToolID(_int CurToolID) { m_PlayerState.iCurToolID = CurToolID; }
	_uint* GetFoodIconID() { return m_iFoodIconID; };
	_float* GetFoodDutation() { return m_fFoodBuffTimeAcc; }
	_bool IsActivateFoodBuff(FOOD_BUFF eFoodType) { return m_bFoodBuff[eFoodType]; }
	void UseFoodBuff(FOOD_BUFF eFoodType, _uint iFoodIconID);
	void AddHP(CHARACTERS eCharater, _float fValue);
	void AddExp(CHARACTERS eCharater, _float fExp);
	CHARACTER_STATE* Get_MainCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_MAIN]]); }
	CHARACTER_STATE* Get_SecondCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_SUB1]]); }
	CHARACTER_STATE* Get_ThirdCharacterState() { return &(m_CharacterState[m_CharSlot[SLOT_SUB2]]); }
	PLAYER_STATE* Get_PlayerState() { return &(m_PlayerState); }
	void AddPlayer() { if (4 > m_PlayerState.iCharCount) { ++m_PlayerState.iCharCount; } }

	//Resonance Chain
	enum SQ_TYPE { SQUENCE1, SQUENCE2, SQUENCE3, SQUENCE4, SQUENCE_END };
	void SetSequenceLevel(SQ_TYPE eType, _uint iGrade);

	const ATTACK_DESC* GetAttackDesc(CHARACTERS eCharacter) const { return &m_AttackDesc[eCharacter]; };
	void SetAttackFactor(CHARACTERS eCharacter, _uint iAttack, _float fFactor);
	void SeBurstFactor(CHARACTERS eCharacter, _float fFactor);
	void SetAirboneQTE(CHARACTERS eCharacter, _bool bAirbone);
	_uint GetCharacterSP(CHARACTERS eCharacter) { return m_iSkillPoint[eCharacter]; }

	void SetCamBehind();

private:
	void levelUp(CHARACTERS eCharater);

public:
	static CPlayerState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	// ĳ���Ͱ� �ƴ� �÷��̾� ��ü�� ���� ������ ���� ����ü
	PLAYER_STATE		m_PlayerState;
	// �� ĳ���ͺ� ����, ���� ���� : (��� -> ��� -> ġ��)
	CHARACTER_STATE		m_CharacterState[CHARACTER_END];
	// ĳ���� ������
	CCharacter*			m_pCharacter[CHARACTER_END] = { nullptr, };
	// �÷��̾� ī�޶� ������
	class CPlayerCamera* m_pPlayerCam = { nullptr };
	// ĳ���� ���� ��Ȳ, enum ID���� ������� �����س���, ĳ���� ��ü�ϴ� ������ ���� ��ġ�� �ٲ�
	CHARACTERS			m_CharSlot[SLOT_END];

	// ����� ����
	CItem::ITEM_DESC	m_EquipWeapons[CHARACTER_END];

	// ���ݷ�, ġ��Ÿ, ����, ü�� : ���� ���(0/3)
	_float m_fSequenceValue[SQUENCE_END] = { 1.f, 1.f, 1.f, 1.f };
	
	_uint m_iSkillPoint[CHARACTER_END] = { 0, };
	ATTACK_DESC m_AttackDesc[CHARACTER_END];

	_float m_fFoodBuffTimeAcc[FOOD_END] = { 0.f, 0.f, 0.f };
	_uint m_iFoodIconID[FOOD_END] = { 0, 0, 0 };
	_bool m_bFoodBuff[FOOD_END] = { false, false, false };
	
};

END

