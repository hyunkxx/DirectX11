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
	// �׺� �޽� Cell Type
	enum CELL
	{
		CELL_GROUND,
		CELL_WALL,
		CELL_END
	};

	// AnimSet Ÿ��
	enum AnimSets
	{
		ANIMSET_BASE,
		ANIMSET_RIBBON,
		ANIMSET_END
	};

	// �÷��̾� ĳ���͵��� ������ ���µ�, �⺻ ������
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
		SS_BEHIT_FLY_START, // or PUSH  ~~ ���� �����
		SS_BEHIT_FLY_LOOP,
		SS_BEHIT_FLY_FALL,
		SS_BEHIT_PUSH, // �ٴڿ� �Ĺ���
		SS_DEATH_IN_WATER, // �ͻ�
		SS_DEAD, // �Ϲ� ���
		SS_END
	};

	// ��� � ������
	enum StatePhysics
	{
		SP_NONE,

		SP_WALK_F,
		SP_RUN_F,
		SP_SPRINT_F,
		SP_SPRINT_B,
		SP_FALL,
		SP_PLAYERGIRL_AIRATTACK,

		// ���ǰ� ���� �ߴ� ������
		SP_BEHIT_FLY_START,
		// ���� �ǰ�, ���ִ� ���� ����
		SP_BEHIT_HOVER,
		// ���ǰ�, �ڷ� �� �и��� ������
		SP_BEHIT_PUSH,
		// BEHIT_FLY �÷��̾� ����
		SP_BEHIT_FLY_PLAYER,
		SP_END,

	};

	// NONE : ȸ�� ����, ONSTART : ���� ���� �� 1ȸ Ÿ�� ������ �ٶ�, LOOKAT : ���� ���� �� �� ������ Ÿ�� ������ �ٶ�
	// TURN : ���� ���� �� �� ������ Ÿ�� �������� Transform�� ȸ�� ���ӵ� ��ŭ ȸ����
	enum StateRotationType
	{
		ROT_NONE,
		ROT_ONSTART,
		ROT_LOOKAT,
		ROT_TURN,
		ROT_END,
	};

	// ĳ������ ���� ��ġ ����, ����, ����, ��
	enum PositionState
	{
		PS_GROUND,
		PS_AIR,
		PS_CLIMB,
		PS_SWIM,
		PS_END
	};

	// �浹 Ÿ��
	enum CollisionType
	{
		CT_NONE,
		CT_PLAYER,
		CT_MONSTER,
		CT_END
	};

	// ���� ���� ����ü, model/base/ribbon �и��� ���̽�(VTF)
	typedef struct tagMultiAnimStateInfo
	{
		_uint iAnimID[ANIMSET_END];
		
		// �� �ִϸ��̼��� ���������� ����ǰ� ������ �� ������ ���� ����
		_uint iNextState;


		// NONE : ȸ�� ����, ONSTART : ���� ���� �� 1ȸ Ÿ�� ������ �ٶ�, LOOKAT : ���� ���� �� �� ������ Ÿ�� ������ �ٶ�
		// TURN : ���� ���� �� �� ������ Ÿ�� �������� Transform�� ȸ�� ���ӵ� ��ŭ ȸ����
		_uint iRotationType;

		_double FramePerSec; // == TicksPerSec

		// �ݺ� ����, true�� ������ ���� �� ������ �ݺ���
		_bool bLoop;
		// ���� ����
		_bool bLerp;
		// Root ��� ����
		_bool bRootMotion;
		// ������ ��Ÿ�� ���� �� 
		_bool bApplyCoolTime;
		// �����, ������
		_bool bWeaponState;
		// ��Ÿ�� �� 
		_double CoolTime;
		_uint iPhysicMoveID; // RootMotion�� false �� ��� ����� ��� � ������ ID
		_uint iEnterPriority; // ���� �켱����
		_uint iLeavePriority; // ��Ż �켱����
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}MULTISTATE;
	
	// ���� ���� ����ü, model 1�� ¥��(Anim) 
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
		_uint iEnterPriority;
		_uint iLeavePriority;
		// StateKeys
		_uint iKeyCount;
		CStateKey** ppStateKeys;
	}SINGLESTATE;

	// ���� ������ ���� ����ü
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
	
	// ĳ������ �� ���¸� �����ϴ� ����ü
	struct StateController
	{
		_uint			iCurState;		// ���� ���� ID
		_uint			iNextState;		// ���� ���� ID > Setup_State ȣ�� �� �� ���·� �Ѿ�� ��
		_double			TrackPos;		// ���� ��� ���� �ִϸ��̼��� Ʈ��������(���� '������' �� ~~ 24fps ~~ 1 frame == 1/24 sec)
		_uint			iPrevCellState;	// �� ������ �׺� �� Ÿ�� ����
		PositionState	ePositionState;
		_bool			bAnimFinished;
		_float3			vMovement;
		_float3			vPrevMovement;
		_bool			bWalk;
		_bool			bFalling;
		//_float4x4		DefaultMatrix; // �ִϸ��̼� ���� ���� ���
	};

	// �ǰ� �ִϸ��̼� ����
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

	// ���� ������ �����ϴ� ����ü
	typedef struct tagAttackInformation
	{
		_float	fDamageFactor;	// ����� ���
		ELEMENT	eElementType;	// ���� �Ӽ�
		HIT		eHitIntensity;	// �ǰ� ����
		_float	fSPGain;		// Special ������ ȸ����
		_float	fBPGain;		// Burst ������ ȸ����
		_float	fTPGain;		// QTE ������ ȸ����
		_tchar  szHitEffectTag[MAX_PATH]; // ��Ʈ ����Ʈ �±�
		_uint	iHitEffectID;	// ��Ʈ ����Ʈ ���̾� ID
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


public: // StateKey ���� �Լ� ����
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

	// ������ ����
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

