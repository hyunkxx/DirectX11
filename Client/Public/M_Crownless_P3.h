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
class CM_Crownless_P3
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
		IS_IDLE, // ���� �ൿ ����
		IS_STEP_BACK,	//Attack04 
		IS_STEP_L,		//Attack03
		IS_STEP_R,		//Attack02
		IS_WALK_F,
		IS_WALK_L,
		IS_WALK_R,
		IS_ATTACK01,	// (�߰Ÿ�) ���� 3��Ÿ
		IS_ATTACK05,	// (��~��) ���� ���� �� ���� ����ü �߻�
		IS_ATTACK06,	// (��~��) ������ ���� �� ���� ����ü �߻�
		IS_ATTACK07,	// (�߰Ÿ�) ���� �ֵθ���
		IS_ATTACK08,	// (�ٰŸ�) ������ �������
		IS_ATTACK09,	// (�ٰŸ�) (Attack08 ��Ʈ ��� ����) �ݹ��� ���� ���â
		IS_ATTACK10,	// (�ٰŸ�) 3��Ÿ â �ֵθ���
		IS_ATTACK11,	// (�ٰŸ�) 2Ÿ â �ֵθ��� ���
		IS_ATTACK12,	// (�߰Ÿ�) ���� ���
		IS_ATTACK13,	// ��������
		IS_STANDUP,
		IS_BEHIT_B,		// Big
		IS_BEHIT_BLOCK, // Block
		IS_BEHIT_FLY_START,
		IS_BEHIT_FLY_LOOP,
		IS_BEHIT_FLY_FALL,
		IS_BEHIT_HOVER, // �� ä�� ����
		IS_PARALYSIS_START, // �����
		IS_PARALYSIS_LOOP,
		IS_PARALYSIS_END,
		IS_DEAD,
		IS_END
	};

	enum MeleeAttacks
	{
		MA_ATTACK08,
		MA_ATTACK11,
		MA_ATTACK10,
		MA_END
	};

	// ����Ʈ�� �� ���� ��� �迭 �ε��� 
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		EBONE_HEAD,
		// ������� ���� �������� ����

		EBONE_LFOOT,
		EBONE_RFOOT,

		EBONE_WEAPON,

		
		EBONE_END
	};

	// ���� ����
	enum Attacks
	{
		ATK_ATTACK01_1,
		ATK_ATTACK01_2,
		ATK_ATTACK01_3,
		ATK_ATTACK05, // 06�̶� ����
		ATK_ATTACK07,
		ATK_ATTACK08,
		ATK_ATTACK09,
		ATK_ATTACK10_1,
		ATK_ATTACK10_2,
		ATK_ATTACK10_3,
		ATK_ATTACK11_1,
		ATK_ATTACK11_2,
		ATK_ATTACK12,
		ATK_ATTACK13_1,
		ATK_ATTACK13_2,
		ATK_ATTACK13_3,
		ATK_ATTACK13_4,
		ATK_ATTACK13_5,
		ATK_ATTACK13_6,
		ATK_ATTACK13_7,
		ATK_ATTACK13_8,
		ATK_ATTACK13_9,
		ATK_PARALYSIS_END_1,
		ATK_PARALYSIS_END_2,
		ATK_END
	};

	// �̻��� ����
	enum Missiles
	{
		MISS_ATTACK01_3,
		MISS_ATTACK05,
		MISS_ATTACK06,
		MISS_ATTACK08,
		MISS_ATTACK13_7,
		MISS_END
	};

private:
	CM_Crownless_P3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Crownless_P3(const CM_Crownless_P3& rhs);
	virtual ~CM_Crownless_P3() = default;

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
	// AnimTool���� ������ State ������ �ε�
	static HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// State ���� ����
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

	

public: // StateKey ���� �Լ� ����
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

	virtual void Set_AttackHit(_bool bAttackHit)
	{
		m_bAttackHit = bAttackHit;
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

	// Trace > �ܻ�
	static const _uint	m_iTraceCount = 20;
	static const _double m_TraceInterval; // _double�� In-class Initializer �ȵȴٰ� ��
	_double				m_TraceTimeAcc = 0.0;
	TRACE				m_TraceArray[m_iTraceCount] = {};

	_double				m_TraceDuration = 0.0;
	_bool				m_bTraceOn = false;

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};

	// ���� ����ü
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB ��Ʈ �� ����� ���� ����üID

												// �̻��� Ǯ
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr, };
	_float3				m_MissileRotAngles[MISS_END];

	// ���� ����
	MONINFO				m_tMonsterInfo;
	_bool				m_bParryable = { false };
	// Ÿ�� �÷��̾� > ������ �� Start���� �÷��̾� �־���
	CCharacter*			m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	// �÷��̾� ���� ����
	_bool				m_bAlert = { false };
	// ������ �÷��̾� ���� �Ÿ�
	_float				m_fAlertRange = { 0.f };
	// ���Ϳ� �÷��̾� ���� �Ÿ�
	_float				m_fTargetDistance = { 0.f };

	// ���� �ൿ�� ���� �۷ι� ��Ÿ�� ����
	_double				m_GlobalCoolTime = { 0.0 };
	_bool				m_bAttackReady = { false };

	_bool				m_bAttackHit = { false };

	_uint				m_iMeleeAttackID = { MA_ATTACK08 };
	_uint				m_iMeleeStartAttackArray[MA_END] = {};
	_uint				m_iMeleeAttackStack = {};

	// MoveCollider �浹 �� ���� ����
	// �и��� �Ÿ� = ��ģ �Ÿ� * (1 - �� ���� / (��� ���� + �� ����))
	_float				m_fPushWeight = {};

	//
	_float3				m_vLevelCenterPosition = {};
	_float				m_fHitPoint = {};
	_float				m_fParalysisPoint = {};
	_double				m_ParalysisTimeAcc = {};

	//
	_double				m_DodgeTimeAcc = { 0.0 };
	_uint				m_iDodgeCount = { 0 };

	//UI�߰�
	class CUI_Minimap*		m_pUIIcon = { nullptr };
	_int					m_UIIndex = { 0 };
	CUI_Monster*			m_pUIMon = { nullptr };

private:
	HRESULT Add_Components();
	void SetUp_State();
	// �÷��̾ ���� �Ÿ� ������ �����ߴ��� Ȯ���ϴ� �Լ�
	void Find_Target();
	void Check_AttackHit();

	void Init_AttackInfos();
	void Init_Missiles();

	void Init_Traces();

	// ���� ���� ��Ÿ�� TimeDelta ��ŭ �ٿ��ִ� �Լ�
	void Apply_CoolTime(_double TimeDelta);
	// ���� �ൿ(����) ����
	void Select_State(_double TimeDelta);
	// ���¿� ���� �ൿ ����
	void Tick_State(_double TimeDelta);
	// ������ ���� ���� ó��
	void On_Cell();
	// 
	void On_Hit(CCharacter* pChar, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos);

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CM_Crownless_P3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }

	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }

	void CM_Crownless_P3::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P3::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P3::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	class CCameraMovement* m_pCamMovement = nullptr;
};

END



