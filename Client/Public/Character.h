#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "StateKey.h"
#include "Collider.h"


BEGIN(Client)

class CCharacter abstract 
	: public CGameObject
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
public:
	// 네비 메쉬 Cell Type
	enum CELL
	{
		CELL_GROUND,
		CELL_WALL,
		CELL_END
	};

	// AnimSet 타입
	enum AnimSets
	{
		ANIMSET_BASE,
		ANIMSET_RIBBON,
		ANIMSET_END
	};

	// 플레이어 캐릭터들이 공유할 상태들, 기본 움직임
	enum PlayerCharacterSharedStates
	{
		// 정지
		SS_STAND1,
		SS_STAND1_ACTION01,
		SS_STAND1_ACTION02,
		SS_STAND1_ACTION03,
		SS_STAND2,
		SS_STANDCHANGE, // 납도 같음 > Stand1로
		SS_STANDUP, // 일어나기 > Stand1로
		// 이동 - 걷기
		SS_WALK_F,
		SS_WALK_B,
		SS_WALK_LF,
		SS_WALK_LB,
		SS_WALK_RF,
		SS_WALK_RB,
		SS_WALK_STOP_L,
		SS_WALK_STOP_R,
		// 이동 - 달리기
		SS_RUN_F,
		SS_RUN_B,
		SS_RUN_LF,
		SS_RUN_LB,
		SS_RUN_RF,
		SS_RUN_RB,
		SS_RUN_TURNBACK, //방향 전환
		SS_RUN_STOP_L,
		SS_RUN_STOP_R,
		// 이동 - 질주
		SS_SPRINT,
		SS_SPRINT_IMPULSE_F, // 고개 숙이고 전력질주
		SS_SPRINT_STOP_L,
		SS_SPRINT_STOP_R,
		// 회피 / 저스트 회피
		SS_MOVE_F, 
		SS_MOVE_B,
		SS_MOVE_LIMIT_F, 
		SS_MOVE_LIMIT_B,
		// 점프
		SS_JUMP_WALK,
		SS_JUMP_RUN,
		SS_JUMP_SECOND_F, // 옆으로 회전
		SS_JUMP_SECOND_B, // 백덤블링
		// 착지
		SS_LAND_LIGHT,
		SS_LAND_HEAVY,
		SS_LAND_ROLL, // 구르기 착지
		// 경사면 슬라이딩
		SS_LANDSLIDE_F, 
		SS_LANDSLIDE_B,
		SS_LANDSLIDE_L,
		SS_LANDSLIDE_R,
		// 갈고리 액션
		SS_FIXHOOK_START_1_D, // 지상에서 시작
		SS_FIXHOOK_START_1_F,
		SS_FIXHOOK_START_1_U,
		SS_FIXHOOK_START_2_D, // 공중에서 시작
		SS_FIXHOOK_START_2_F,
		SS_FIXHOOK_START_2_U,
		SS_FIXHOOK_LOOP_D,
		SS_FIXHOOK_LOOP_F,
		SS_FIXHOOK_LOOP_L,
		SS_FIXHOOK_LOOP_R,
		SS_FIXHOOK_LOOP_U,
		SS_FIXHOOK_END_UP,		// 날아가면서
		SS_FIXHOOK_END,		//  다리만 들어올림 - 스파이더맨 자세
		// 낙하
		SS_FALL,
		SS_FALL_FAST,
		// 수영
		SS_SWIM_FALL, // 시작
		SS_SWIM_STAND,
		SS_SWIM_LOOP_F, // 평영
		SS_SWIM_LOOP_L,
		SS_SWIM_LOOP_R,
		SS_SWIMUP_LOOP_F, // 자유형
		SS_SWIMUP_LOOP_L,
		SS_SWIMUP_LOOP_R,
		// 벽 타기
		SS_CLIMB_START_UP,
		SS_CLIMB_START_DOWN,
		SS_CLIMB_STAND,
		SS_CLIMB_D1,
		SS_CLIMB_D1_STOP,
		SS_CLIMB_D2,
		SS_CLIMB_D2_STOP,
		SS_CLIMB_DL1,
		SS_CLIMB_DL1_STOP,
		SS_CLIMB_DL2,
		SS_CLIMB_DL2_STOP,
		SS_CLIMB_DR1,
		SS_CLIMB_DR1_STOP,
		SS_CLIMB_DR2,
		SS_CLIMB_DR2_STOP,
		SS_CLIMB_L1,
		SS_CLIMB_L1_STOP,
		SS_CLIMB_L2,
		SS_CLIMB_L2_STOP,
		SS_CLIMB_R1,
		SS_CLIMB_R1_STOP,
		SS_CLIMB_R2,
		SS_CLIMB_R2_STOP,
		SS_CLIMB_U1,
		SS_CLIMB_U1_STOP,
		SS_CLIMB_U2,
		SS_CLIMB_U2_STOP,
		SS_CLIMB_UL1,
		SS_CLIMB_UL1_STOP,
		SS_CLIMB_UL2,
		SS_CLIMB_UL2_STOP,
		SS_CLIMB_UR1,
		SS_CLIMB_UR1_STOP,
		SS_CLIMB_UR2,
		SS_CLIMB_UR2_STOP,
		SS_CLIMB_MOVE,
		SS_CLIMB_ONTOP,
		SS_CLIMB_VAULT, // 장애물 뛰어넘기?
		// 벽 점프 > 해당 방향으로 1회성 점프
		SS_CLIMB_DASH_D,
		SS_CLIMB_DASH_DL,
		SS_CLIMB_DASH_DR,
		SS_CLIMB_DASH_L,
		SS_CLIMB_DASH_R,
		SS_CLIMB_DASH_U,
		SS_CLIMB_DASH_UL,
		SS_CLIMB_DASH_UR,
		// 벽 달리기 > 계속 달림
		SS_CLIMB_BOOST_L,
		SS_CLIMB_BOOST_L_START,
		SS_CLIMB_BOOST_L_STOP,
		SS_CLIMB_BOOST_R,
		SS_CLIMB_BOOST_R_START,
		SS_CLIMB_BOOST_R_STOP,
		SS_CLIMB_BOOST_U,
		SS_CLIMB_BOOST_U_START,
		SS_CLIMB_BOOST_U_STOP,
		SS_CLIMB_BOOST_ONTOP,		// 등반 완료 후
		SS_CLIMB_BOOST_ONTOPTOSPRINT, // > 질주 속행
		SS_CLIMB_BOOST_ONTOPTOSTOP,	// > 멈춰 서기
		SS_CLIMB_BOOST_VAULT,		// 장애물 뛰어넘기?
		SS_CLIMB_BOOST_VAULTTOFALL,
		// 갈고리? 스윙 액션, LOOP는 리소스 이슈로 폐기
		SS_SWING_START, // 
		SS_SWING_END,
		// 호리병 던지기
		SS_MANIPULATE_START,
		SS_MANIPULATE_RELEASE,
		SS_MANIPULATE_LOOP,
		SS_MANIPULATE_END,
		//  각종 상호작용
		SS_SUMMON, // 에코 소환?
		SS_THROW_D, // 뭔가 던짐
		SS_THROW_F,
		SS_THROW_U,
		SS_CONTROL_D, // 상호작용(왼손 뻗기)
		SS_CONTROL_F,
		SS_CONTROL_U,
		// 피격 ~ 사망
		SS_BEHIT_S, // Small
		SS_BEHIT_B, // Big
		SS_BEHIT_FLY_START, // or PUSH  ~~ 둘이 비슷함
		SS_BEHIT_FLY_LOOP,
		SS_BEHIT_FLY_FALL,
		SS_BEHIT_PUSH, // 바닥에 쳐박힘
		SS_DEATH_IN_WATER, // 익사
		SS_DEAD, // 일반 사망
		SS_END
	};

	// 등가속 운동 움직임
	enum StatePhysics
	{
		SP_NONE,

		SP_WALK_F,
		SP_RUN_F,
		SP_SPRINT_F,
		SP_SPRINT_B,
		SP_FALL,
		SP_PLAYERGIRL_AIRATTACK,

		// 강피격 위로 뜨는 날리기
		SP_BEHIT_FLY_START,
		// 공중 피격, 떠있는 상태 지속
		SP_BEHIT_HOVER,
		// 강피격, 뒤로 쭉 밀리는 날리기
		SP_BEHIT_PUSH,
		// BEHIT_FLY 플레이어 버전
		SP_BEHIT_FLY_PLAYER,
		SP_END,

	};

	// NONE : 회전 안함, ONSTART : 상태 진입 시 1회 타겟 방향을 바라봄, LOOKAT : 상태 적용 중 매 프레임 타겟 방향을 바라봄
	// TURN : 상태 적용 중 매 프레임 타겟 방향으로 Transform의 회전 각속도 만큼 회전함
	enum StateRotationType
	{
		ROT_NONE,
		ROT_ONSTART,
		ROT_LOOKAT,
		ROT_TURN,
		ROT_END,
	};

	// 캐릭터의 현재 위치 상태, 지상, 공중, 벽
	enum PositionState
	{
		PS_GROUND,
		PS_AIR,
		PS_CLIMB,
		PS_SWIM,
		PS_END
	};

	// 충돌 타입
	enum CollisionType
	{
		CT_NONE,
		CT_PLAYER,
		CT_MONSTER,
		CT_END
	};

	// 상태 정의 구조체, model/base/ribbon 분리된 케이스(VTF)
	typedef struct tagMultiAnimStateInfo
	{
		_uint iAnimID[ANIMSET_END];
		
		// 이 애니메이션이 정상적으로 재생되고 끝났을 때 진입할 다음 상태
		_uint iNextState;


		// NONE : 회전 안함, ONSTART : 상태 진입 시 1회 타겟 방향을 바라봄, LOOKAT : 상태 적용 중 매 프레임 타겟 방향을 바라봄
		// TURN : 상태 적용 중 매 프레임 타겟 방향으로 Transform의 회전 각속도 만큼 회전함
		_uint iRotationType;

		_double FramePerSec; // == TicksPerSec

		// 반복 여부, true면 가만히 뒀을 때 영원히 반복함
		_bool bLoop;
		// 보간 여부
		_bool bLerp;
		// Root 모션 여부
		_bool bRootMotion;
		// 상태의 쿨타임 진입 시 
		_bool bApplyCoolTime;
		// 사용중, 수납중
		_bool bWeaponState;
		// 쿨타임 값 
		_double CoolTime;
		_uint iPhysicMoveID; // RootMotion이 false 일 경우 사용할 등가속 운동 움직임 ID
		_uint iEnterPriority; // 진입 우선순위
		_uint iLeavePriority; // 이탈 우선순위
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}MULTISTATE;
	
	// 상태 정의 구조체, model 1개 짜리(Anim) 
	typedef struct tagSingleAnimStateInfo
	{
		_uint iAnimID;
		//
		_uint iNextState;
		_uint iRotationType;
		_double FramePerSec; // == TicksPerSec
		_bool bLoop;
		_bool bLerp;
		_bool bRootMotion;
		_bool bApplyCoolTime;
		_bool bWeaponState;
		_double CoolTime;
		_uint iPhysicMoveID; // RootMotion이 false 일 경우 사용할 등가속 운동 움직임 ID
		_uint iEnterPriority;
		_uint iLeavePriority;
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}SINGLESTATE;

	// 물리 움직임 정의 구조체
	typedef struct tagPhysicMove
	{
		_bool	bInitMovement;			// true == 운동 시작 시, InitDir/Force 적용, false == 운동 시작 시, 직전 운동 상태 유지
		_bool	bConstant;				// true == 등속 운동, 시작 운동 상태가 변하지 않음, false == 운동 상태가 변함
		_float3 vInitDir;				// 시작 운동 방향 플레이어 로컬축 Normalized
		_float	fInitForce;				// 시작 운동 속도 /s
		_float	fHorizontalAtten;		// 가로축 운동 속도 감쇄율 - 마찰
		_float	fVerticalAccel;			// 세로축 가속도 - 중력
		_float	fHorizontalMaxSpeed;	// 가로축(XZ) 최대 속도 절대값
		_float	fVerticalMaxSpeed;		// 세로축(Y) 최대 속도 절대값
	}PHYSICMOVE;
	
	// 캐릭터의 현 상태를 정의하는 구조체
	struct StateController
	{
		_uint			iCurState;		// 현재 상태 ID
		_uint			iNextState;		// 다음 상태 ID > Setup_State 호출 시 이 상태로 넘어가게 됨
		_double			TrackPos;		// 현재 재생 중인 애니메이션의 트랙포지션(누적 '프레임' 값 ~~ 24fps ~~ 1 frame == 1/24 sec)
		_uint			iPrevCellState;	// 전 프레임 네비 셀 타입 저장
		PositionState	ePositionState;
		_bool			bAnimFinished;
		_float3			vMovement;
		_float3			vPrevMovement;
		_bool			bWalk;
		_bool			bFalling;
		//_float4x4		DefaultMatrix; // 애니메이션 시작 상태 행렬
	};

	// 피격 애니메이션 강도
	typedef enum HitIntensity
	{
		HIT_NONE,
		HIT_SMALL,
		HIT_BIG,
		HIT_FLY,
		HIT_PUSH,
		HIT_END
	}HIT;

	typedef struct tagMonsterInformation
	{
		_tchar	szName[MAX_PATH];
		ELEMENT	eElement;
		_uint	iLevel;
		_uint	iExp;
		_float	fMaxHP;
		_float	fCurHP;
		_float	fAttack;
		_float	fDefense;
		_float	fCriticalRate;
		_float	fCriticalDMG;
	}MONINFO;

	// 공격 정보를 정의하는 구조체
	typedef struct tagAttackInformation
	{
		_float	fDamageFactor;	// 대미지 계수
		ELEMENT	eElementType;	// 공격 속성
		HIT		eHitIntensity;	// 피격 강도
		_float	fSPGain;		// Special 게이지 회복량
		_float	fBPGain;		// Burst 게이지 회복량
		_float	fTPGain;		// QTE 게이지 회복량
		_tchar  szHitEffectTag[MAX_PATH]; // 히트 이펙트 태그
		_uint	iHitEffectID;	// 히트 이펙트 레이어 ID
	}TAGATTACK;

	typedef struct tagTraceDesc
	{
		_float4x4	TraceWorldMatrix;
		_float4x4**	ppTraceBoneMatrices;
		_double		TraceDuration;
		_double		TraceTimeAcc;
		_double		FadeInRate;
		_double		FadeOutRate;
		_bool		bRendering;
	}TRACE;

public:
	virtual void Check_Nearst(CCharacter* pChar, _float fDist) {}
	virtual CCollider* GetAttackCollider() const { return nullptr; }
	virtual CCollider* GetHitCollider() const { return nullptr; }
	virtual CCollider* GetMoveCollider() const { return nullptr; }
	const _uint	Get_CollType() { return m_eCollisionType; }

	virtual _uint Get_AttackID() { return 0; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* fAttackOut) {}

	virtual _vector Get_Position();
	virtual _vector Get_MainWeaponPosition() { return XMVectorZero(); }
	void Push_OnNavi(_fvector PushVector);

	virtual _float Get_PushWeight() { return 0.f; }

	virtual _float Get_CoolTime(_uint iType) { return 0.f; }

	virtual _bool Get_Attack() { return false; }

	virtual void Set_TimeDelay(_double DelayDuration, _double DelayValue) {}

	virtual void Set_AttackHit(_bool bAttackHit) {}

	virtual void Recover_Gauge(_float fSP, _float fBP, _float fTP) {}


public: // StateKey 대응 함수 모음
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _float fDissSpeed) {}
	virtual void Shot_PriorityKey(_uint iLeavePriority) {}
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint iEffectBoneID, _uint iTypeID, _bool bTracking) {}
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID) {}
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID) {}
	virtual void Shot_TraceKey(_bool bTraceOn, _bool bOnce, _double Duration) {}
	
	//
	virtual void Shot_SlowKey(_float fTargetTime, _float fLerpSpeed);
	virtual void Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed);

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

	static PHYSICMOVE StatePhysics[SP_END];

	//UI
	static _int Monindex;
protected:
	StateController m_Scon;
	CollisionType	m_eCollisionType;
	_bool			m_bRender = true;

	// 디졸브 벨류
	_float m_fDissolveAmount = 0.f;
	_float m_fDissolveTimeAcc = 0.f;
	_bool m_bDissolve = false;
	_bool m_bDissolveType = false;
	_float m_fDissolveSpeed = 1.f;
	_float3 m_vDissolveColor = {};

public:
	virtual void Free() override;
};

END

