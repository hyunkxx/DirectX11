#pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "MissilePool.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CBone;
class CNavigation;
END

BEGIN(Client)
class CUI_Monster;
class CM_Crownless_P1
	: public CCharacter
{
public:
	enum AI_STATE
	{
		AI_NONE,
		AI_IDLE,
		AI_CHASE,
		AI_DODGE,
		AI_STAY,
		AI_ATTACK_MELEE,
		AI_ATTACK_RANGE,
		AI_END
	};

	enum IndividualStates
	{
		IS_IDLE, // 다음 행동 선택
		IS_TAUNT,
		IS_BACKSTEP,	//Attack_01
		IS_SIDESTEP_L,	//Attack_06
		IS_SIDESTEP_R,	//Attack_07
		IS_WALK_F,
		IS_WALK_L,
		IS_WALK_R,
		IS_ATTACK02,
		IS_ATTACK03,
		IS_ATTACK04,
		IS_ATTACK05,
		IS_ATTACK08,
		IS_STANDUP,
		IS_BEHIT_S, // Small
		IS_BEHIT_B, // Big
		IS_BEHIT_FLY_START,
		IS_BEHIT_FLY_LOOP,
		IS_BEHIT_FLY_FALL,
		IS_BEHIT_HOVER, // 뜬 채로 맞음
		IS_DEAD,
		IS_WAIT,		// 페이즈 전환 연출 대기
		IS_END
	};

	// 이펙트용 본 월드 행렬 배열 인덱스 
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		EBONE_HEAD,
		// 여기까지 몬스터 공통으로 고정
		EBONE_LFOOT,
		EBONE_RFOOT,

		EBONE_END
	};

	// 공격 종류
	enum Attacks
	{
		ATK_ATTACK_02,
		ATK_ATTACK_03,
		ATK_ATTACK_04_1,
		ATK_ATTACK_04_3,
		ATK_ATTACK_05,
		ATK_ATTACK_08,
		ATK_END
	};

	// 미사일 종류
	enum Missiles
	{
		MISS_ATTACK_03,
		MISS_ATTACK_05,
		MISS_ATTACK_08,
		MISS_END
	};

private:
	CM_Crownless_P1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Crownless_P1(const CM_Crownless_P1& rhs);
	virtual ~CM_Crownless_P1() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

	// static
	// AnimTool에서 저장한 State 정보를 로드
	static HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// State 정보 삭제
	static void Release_States();

	void Trace_On()
	{
		m_bTraceOn = true;
		m_TraceTimeAcc = m_TraceInterval;
	}
	void Trace_Off()
	{
		m_bTraceOn = false;
	}

	void Shot_Trace(_double Duration, _double FadeInRate = 0.15, _double FadeOutRate = 0.85);

	void Release_Traces();

public: // StateKey 대응 함수 모음
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID, _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);
	virtual void Shot_TraceKey(_bool bTraceOn, _bool bOnce, _double TraceDuration)
	{
		if (false == bOnce)
		{
			m_bTraceOn = bTraceOn;
			m_TraceDuration = TraceDuration;
		}
		else
		{
			Shot_Trace(TraceDuration);
		}
	}

public:
	virtual _uint Get_AttackID() override { return m_iCurAttackID; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* pAttackOut) override
	{
		memcpy(pAttackInfoOut, &m_AttackInfos[iAttackID], sizeof(TAGATTACK));
		*pAttackOut = m_tMonsterInfo.fAttack;
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }

	virtual _float4x4* Get_HeadMatrix()
	{
		if (nullptr == m_EffectBones[EBONE_HEAD])
			return nullptr;

		return &m_EffectBoneMatrices[EBONE_HEAD];
	}

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	// State
	static SINGLESTATE	m_tStates[IS_END];
	SINGLESTATE			m_tCurState;
	_double				m_StateCoolTimes[IS_END] = { 0.0, };

	// Trace > 잔상
	static const _uint	m_iTraceCount = 20;
	static const _double m_TraceInterval; // _double은 In-class Initializer 안된다고 함
	_double				m_TraceTimeAcc = 0.0;
	TRACE				m_TraceArray[m_iTraceCount] = {};

	_double				m_TraceDuration = 0.0;
	_bool				m_bTraceOn = false;

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};

	// 공격 구조체
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB 히트 시 사용할 공격 구조체ID

	// 미사일 풀
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr, };
	_float3				m_MissileRotAngles[MISS_END];

	// 몬스터 변수
	MONINFO			m_tMonsterInfo;
	// 타겟 플레이어 > 생성될 때 Start에서 플레이어 넣어줌
	CCharacter*			m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	// 플레이어 인지 여부
	_bool				m_bAlert = { false };
	// 몬스터의 플레이어 인지 거리
	_float				m_fAlertRange = { 0.f };
	// 몬스터와 플레이어 간의 거리
	_float				m_fTargetDistance = { 0.f };

	// 공격 행동에 대한 글로벌 쿨타임 변수
	_double				m_GlobalCoolTime = { 0.0 };
	_bool				m_bAttackReady = { false };

	// MoveCollider 충돌 시 비교할 무게
	// 밀리는 거리 = 겹친 거리 * (1 - 내 무게 / (상대 무게 + 내 무게))
	_float				m_fPushWeight = {};

	//
	_float3				m_vLevelCenterPosition = {}; 
	_uint				m_iTauntStack = { 0 };
	_float				m_fHitPoint = {};

	//
	_double				m_DodgeTimeAcc = { 0.0 };
	_uint				m_iDodgeCount = { 0 };

	//UI추가
	class CUI_Minimap*		m_pUIIcon = { nullptr };
	_int					m_UIIndex = { 0 };
	CUI_Monster*			m_pUIMon = { nullptr };

private:
	HRESULT Add_Components();
	void SetUp_State();
	virtual void Change_Target(CCharacter* pActiveCharacter);
	// 플레이어가 일정 거리 안으로 접근했는지 확인하는 함수
	void Find_Target();

	void Init_AttackInfos();
	void Init_Missiles();
	void Init_AnimSystem();

	void Init_Traces();

	// 적용 중인 쿨타임 TimeDelta 만큼 줄여주는 함수
	void Apply_CoolTime(_double TimeDelta);
	// 다음 행동(상태) 결정
	void Select_State(_double TimeDelta);
	// 상태에 따른 행동 수행
	void Tick_State(_double TimeDelta);
	// 지형에 의한 예외 처리
	void On_Cell();
	// 
	void On_Hit(CCharacter* pChar, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos);

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CM_Crownless_P1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }

	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }

	void CM_Crownless_P1::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P1::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P1::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	class CCameraMovement* m_pCamMovement = nullptr;
};

END

