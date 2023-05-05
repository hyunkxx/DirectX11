#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "StateKey.h"

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
public:
	enum AnimSets
	{
		ANIMSET_BASE,
		ANIMSET_RIBBON,
		ANIMSET_END
	};

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
		SS_BEHIT_FLY, // or PUSH  ~~ 둘이 비슷함
		SS_BEHIT_PRESS, // 바닥에 쳐박힘
		SS_DEATH_IN_WATER, // 익사
		SS_DEATH, // 일반 사망
		SS_END
	};

	enum PlayerStatePhysics
	{
		PSP_WALK_F,
		PSP_WALK_B,
		PSP_WALK_L,
		PSP_WALK_R,

		PSP_RUN_F,
		PSP_RUN_B,
		PSP_RUN_L,
		PSP_RUN_R,

		PSP_SPRINT_F,
		PSP_SPRINT_B,

		PSP_END,

	};
	//
	typedef struct tagMultiAnimStateInfo
	{
		_uint iAnimID[ANIMSET_END];
		//
		_uint iNextState;

		// NONE : 회전 안함, ONSTART : 상태 진입 시 1회 타겟 방향을 바라봄, LOOKAT : 상태 적용 중 매 프레임 타겟 방향을 바라봄
		// TURN : 상태 적용 중 매 프레임 타겟 방향으로 Transform의 회전 각속도 만큼 회전함
		_uint iRotationType;
		_double FramePerSec; // == TicksPerSec
		_bool bLoop;
		_bool bLerp;
		_bool bRootMotion;
		_bool bApplyCoolTime;
		_bool bWeaponState;
		_double CoolTime;
		_uint iPhysicMoveID; // RootMotion이 false 일 경우 사용할 등가속 운동 움직임 ID
		_uint iPriority; // 우선순위
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}MULTISTATE;

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
		_uint iPriority;
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}SINGLESTATE;

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
	
	typedef struct StateController
	{
		_uint	iCurState;
		_uint	iNextState;
		_double	TrackPos;
		_bool	bAnimFinished;
		_float3 vMovement;
		_float3	vPrevMovement;
	}SCON;

public: // StateKey 대응 함수 모음
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration) {};



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

	static PHYSICMOVE PlayerStatePhysics[PSP_END];

protected:
	StateController m_tStateController;

public:
	virtual void Free() override;
};

END

