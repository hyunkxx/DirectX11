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

	// 사용 조건 : 쿨타임(TOOL은 캐릭터와 별도로 공통 쿨타임 적용)
	enum COOLTIMES { COOLTIME_Q, COOLTIME_E, COOLTIME_R, COOLTIME_END };
	// 사용 조건 : 수치
	enum GAUGES { GAUGE_Q, GAUGE_E, GAUGE_R, GAUGE_QTE, GAUGE_END };

	enum TOOLS { TOOL_FIXHOOK, TOOL_LAVITATION, TOOL_SCANNER, TOOL_END};

	typedef struct tagPlayerState{
		// 캐릭터 교대 쿨타임
		_float		fTagMaxCooltime;
		_float		fTagCurCooltime;
		
		// 현재 툴 ID, 툴 별 쿨타임
		_uint		iToolID;
		_float		fToolsMaxCooltime[TOOL_END];
		_float		fToolsCurCooltime[TOOL_END];

		// 캠 락온 여부
		_bool		bCamLockOn;
		
		//// QTE 발동 시 공격 대상
		//class CCharacter*	pQTETarget;

		//// 캐릭터 교대 시 월드 행렬
		//_float4x4	WorldMatrix;
	}PLAYER_STATE;

	// PC 기본 스텟
	typedef struct tagCharacterState
	{
		// 기본 상태 수치
		_tchar		szName[MAX_PATH];
		ELEMENT		eElement;
		_uint		iCurLevel;
		_float		fCurExp;
		_float		fMaxExp;
		
		// 전투 관련 수치
		_float		fAttack;			//기본 공격력
		_float		fAttackEx;			//추가 공격력
		_float		fDefense;			//기본 방어력
		_float		fDefenseEx;			//추가 방어력
		_float		fCriticalRate;
		_float		fCriticalDamage;

		_float		fCurHP;
		_float		fMaxHP;

		// 스킬 게이지
		_float		fCurGauges[GAUGE_END];
		_float		fMaxGauges[GAUGE_END];

		// PC 개인 스킬 별 쿨타임
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
	// 각 캐릭터별 스탯 (방순 -> 양양 -> 치하)
	CHARACTER_STATE m_CharacterState[CHARACTER_END];
	// 캐릭터 슬롯 현황, ID값만 순서대로 저장해놓음, 캐릭터 교체하는 순서에 따라 위치가 바뀜
	CHARACTERS m_CharSlot[SLOT_END];


};

END
