#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayerState final : public CGameObject
{
private:
	enum { MAX_LEVEL = 50 };

public:
	enum CHARACTERS { CHARACTER_PLAYERGIRL, CHARACTER_YANGYANG, CHARACTER_CHIXIA, CHARACTER_END };
	enum CHARSLOT { SLOT_MAIN, SLOT_SUB1, SLOT_SUB2, SLOT_END };

	// ��� ���� : ��Ÿ��(TOOL�� ĳ���Ϳ� ������ ���� ��Ÿ�� ����)
	enum COOLTIMES { COOLTIME_Q, COOLTIME_E, COOLTIME_R, COOLTIME_END };
	// ��� ���� : ��ġ
	enum GAUGES { GAUGE_Q, GAUGE_E, GAUGE_R, GAUGE_QTE, GAUGE_END };

	enum TOOLS { TOOL_FIXHOOK, TOOL_LAVITATION, TOOL_SCANNER, TOOL_END};

	typedef struct tagPlayerState{
		// ĳ���� ���� ��Ÿ��
		_float		fTagMaxCooltime;
		_float		fTagCurCooltime;
		
		// ���� �� ID, �� �� ��Ÿ��
		_uint		iToolID;
		_float		fToolsMaxCooltime[TOOL_END];
		_float		fToolsCurCooltime[TOOL_END];

		// ķ ���� ����
		_bool		bCamLockOn;
		
		//// QTE �ߵ� �� ���� ���
		//class CCharacter*	pQTETarget;

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
		_float		fAttack;			//�⺻ ���ݷ�
		_float		fAttackEx;			//�߰� ���ݷ�
		_float		fDefense;			//�⺻ ����
		_float		fDefenseEx;			//�߰� ����
		_float		fCriticalRate;
		_float		fCriticalDamage;

		_float		fCurHP;
		_float		fMaxHP;

		// ��ų ������
		_float		fCurGauges[GAUGE_END];
		_float		fMaxGauges[GAUGE_END];

		// PC ���� ��ų �� ��Ÿ��
		_float fCurCooltime[COOLTIME_END];
		_float fMaxCooltime[COOLTIME_END];

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
	CHARACTER_STATE* Get_CharState_bySlot(_uint iSlotID);
	CHARACTER_STATE* Get_CharState_byChar(_uint iSlotID);

public: // Set

public:
	void AddExp(CHARACTERS eCharater, _float fExp);

private:
	void levelUp(CHARACTERS eCharater);

public:
	static CPlayerState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	// �� ĳ���ͺ� ���� (��� -> ��� -> ġ��)
	CHARACTER_STATE m_CharacterState[CHARACTER_END];
	// ĳ���� ���� ��Ȳ, ID���� ������� �����س���, ĳ���� ��ü�ϴ� ������ ���� ��ġ�� �ٲ�
	CHARACTERS m_CharSlot[SLOT_END];


};

END
