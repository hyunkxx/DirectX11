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
		// ����
		SS_STAND1,
		SS_STAND1_ACTION01,
		SS_STAND1_ACTION02,
		SS_STAND1_ACTION03,
		SS_STAND2,
		SS_STANDCHANGE, // ���� ���� > Stand1��
		SS_STANDUP, // �Ͼ�� > Stand1��
		// �̵� - �ȱ�
		SS_WALK_F,
		SS_WALK_B,
		SS_WALK_LF,
		SS_WALK_LB,
		SS_WALK_RF,
		SS_WALK_RB,
		SS_WALK_STOP_L,
		SS_WALK_STOP_R,
		// �̵� - �޸���
		SS_RUN_F,
		SS_RUN_B,
		SS_RUN_LF,
		SS_RUN_LB,
		SS_RUN_RF,
		SS_RUN_RB,
		SS_RUN_TURNBACK, //���� ��ȯ
		SS_RUN_STOP_L,
		SS_RUN_STOP_R,
		// �̵� - ����
		SS_SPRINT,
		SS_SPRINT_IMPULSE_F, // �� ���̰� ��������
		SS_SPRINT_STOP_L,
		SS_SPRINT_STOP_R,
		// ȸ�� / ����Ʈ ȸ��
		SS_MOVE_F, 
		SS_MOVE_B,
		SS_MOVE_LIMIT_F, 
		SS_MOVE_LIMIT_B,
		// ����
		SS_JUMP_WALK,
		SS_JUMP_RUN,
		SS_JUMP_SECOND_F, // ������ ȸ��
		SS_JUMP_SECOND_B, // �����
		// ����
		SS_LAND_LIGHT,
		SS_LAND_HEAVY,
		SS_LAND_ROLL, // ������ ����
		// ���� �����̵�
		SS_LANDSLIDE_F, 
		SS_LANDSLIDE_B,
		SS_LANDSLIDE_L,
		SS_LANDSLIDE_R,
		// ���� �׼�
		SS_FIXHOOK_START_1_D, // ���󿡼� ����
		SS_FIXHOOK_START_1_F,
		SS_FIXHOOK_START_1_U,
		SS_FIXHOOK_START_2_D, // ���߿��� ����
		SS_FIXHOOK_START_2_F,
		SS_FIXHOOK_START_2_U,
		SS_FIXHOOK_LOOP_D,
		SS_FIXHOOK_LOOP_F,
		SS_FIXHOOK_LOOP_L,
		SS_FIXHOOK_LOOP_R,
		SS_FIXHOOK_LOOP_U,
		SS_FIXHOOK_END_UP,		// ���ư��鼭
		SS_FIXHOOK_END,		//  �ٸ��� ���ø� - �����̴��� �ڼ�
		// ����
		SS_FALL,
		SS_FALL_FAST,
		// ����
		SS_SWIM_FALL, // ����
		SS_SWIM_STAND,
		SS_SWIM_LOOP_F, // ��
		SS_SWIM_LOOP_L,
		SS_SWIM_LOOP_R,
		SS_SWIMUP_LOOP_F, // ������
		SS_SWIMUP_LOOP_L,
		SS_SWIMUP_LOOP_R,
		// �� Ÿ��
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
		SS_CLIMB_VAULT, // ��ֹ� �پ�ѱ�?
		// �� ���� > �ش� �������� 1ȸ�� ����
		SS_CLIMB_DASH_D,
		SS_CLIMB_DASH_DL,
		SS_CLIMB_DASH_DR,
		SS_CLIMB_DASH_L,
		SS_CLIMB_DASH_R,
		SS_CLIMB_DASH_U,
		SS_CLIMB_DASH_UL,
		SS_CLIMB_DASH_UR,
		// �� �޸��� > ��� �޸�
		SS_CLIMB_BOOST_L,
		SS_CLIMB_BOOST_L_START,
		SS_CLIMB_BOOST_L_STOP,
		SS_CLIMB_BOOST_R,
		SS_CLIMB_BOOST_R_START,
		SS_CLIMB_BOOST_R_STOP,
		SS_CLIMB_BOOST_U,
		SS_CLIMB_BOOST_U_START,
		SS_CLIMB_BOOST_U_STOP,
		SS_CLIMB_BOOST_ONTOP,		// ��� �Ϸ� ��
		SS_CLIMB_BOOST_ONTOPTOSPRINT, // > ���� ����
		SS_CLIMB_BOOST_ONTOPTOSTOP,	// > ���� ����
		SS_CLIMB_BOOST_VAULT,		// ��ֹ� �پ�ѱ�?
		SS_CLIMB_BOOST_VAULTTOFALL,
		// ����? ���� �׼�, LOOP�� ���ҽ� �̽��� ���
		SS_SWING_START, // 
		SS_SWING_END,
		// ȣ���� ������
		SS_MANIPULATE_START,
		SS_MANIPULATE_RELEASE,
		SS_MANIPULATE_LOOP,
		SS_MANIPULATE_END,
		//  ���� ��ȣ�ۿ�
		SS_SUMMON, // ���� ��ȯ?
		SS_THROW_D, // ���� ����
		SS_THROW_F,
		SS_THROW_U,
		SS_CONTROL_D, // ��ȣ�ۿ�(�޼� ����)
		SS_CONTROL_F,
		SS_CONTROL_U,
		// �ǰ� ~ ���
		SS_BEHIT_S, // Small
		SS_BEHIT_B, // Big
		SS_BEHIT_FLY, // or PUSH  ~~ ���� �����
		SS_BEHIT_PRESS, // �ٴڿ� �Ĺ���
		SS_DEATH_IN_WATER, // �ͻ�
		SS_DEATH, // �Ϲ� ���
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

		// NONE : ȸ�� ����, ONSTART : ���� ���� �� 1ȸ Ÿ�� ������ �ٶ�, LOOKAT : ���� ���� �� �� ������ Ÿ�� ������ �ٶ�
		// TURN : ���� ���� �� �� ������ Ÿ�� �������� Transform�� ȸ�� ���ӵ� ��ŭ ȸ����
		_uint iRotationType;
		_double FramePerSec; // == TicksPerSec
		_bool bLoop;
		_bool bLerp;
		_bool bRootMotion;
		_bool bApplyCoolTime;
		_bool bWeaponState;
		_double CoolTime;
		_uint iPhysicMoveID; // RootMotion�� false �� ��� ����� ��� � ������ ID
		_uint iPriority; // �켱����
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
		_uint iPhysicMoveID; // RootMotion�� false �� ��� ����� ��� � ������ ID
		_uint iPriority;
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}SINGLESTATE;

	typedef struct tagPhysicMove
	{
		_bool	bInitMovement;			// true == � ���� ��, InitDir/Force ����, false == � ���� ��, ���� � ���� ����
		_bool	bConstant;				// true == ��� �, ���� � ���°� ������ ����, false == � ���°� ����
		_float3 vInitDir;				// ���� � ���� �÷��̾� ������ Normalized
		_float	fInitForce;				// ���� � �ӵ� /s
		_float	fHorizontalAtten;		// ������ � �ӵ� ������ - ����
		_float	fVerticalAccel;			// ������ ���ӵ� - �߷�
		_float	fHorizontalMaxSpeed;	// ������(XZ) �ִ� �ӵ� ���밪
		_float	fVerticalMaxSpeed;		// ������(Y) �ִ� �ӵ� ���밪
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

public: // StateKey ���� �Լ� ����
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

