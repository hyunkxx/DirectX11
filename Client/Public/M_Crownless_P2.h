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
class CM_Crownless_P2
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
		IS_FORCEIDLE, // Idle ��� 1�� ������ �����ֱ�
		IS_TAUNT,
		IS_SIDESTEP_L,	//Attack02
		IS_WALK_F,
		IS_WALK_L,
		IS_WALK_R,
		IS_ATTACK03,	// �÷�����
		IS_ATTACK05,	// 3Ÿ �޺�
		IS_ATTACK06,	// (Attack7 ���� �� ����, Attack3 ��Ʈ �� Ȯ�� ����) 720�� ��������
		IS_ATTACK07,	// (Attack8 2Ÿ ��Ʈ �� ����)���۸� ��ġ ~ �޸��� ����
		IS_ATTACK08,	// 2�� ����
		IS_ATTACK10,	// Ⱦ�� �ܹ�
		IS_ATTACK11,	// (Attack10 ��Ʈ �� ����) �ö��� ��ű
		IS_ATTACK12,	// �齺�� �� �и� �غ�
		IS_ATTACK17,	// �� ��Ƽ� ��ǳ
		IS_ATTACK18,	// �·��
		IS_ATTACK19,	// ��Ŀű
		IS_ATTACK20,	// 4Ÿ �޺�
		IS_ATTACK21,	// (�·�� miss �� ����) �����
		IS_ATTACK22,	// (�и� �غ� �� �ǰ� �� ����) �Ȳ�ġ 
		
		IS_STANDUP,
		IS_BEHIT_S, // Small
		IS_BEHIT_B, // Big
		IS_BEHIT_FLY_START,
		IS_BEHIT_FLY_LOOP,
		IS_BEHIT_FLY_FALL,
		IS_BEHIT_HOVER, // �� ä�� ����
		IS_DEAD,
		IS_WAIT,		// ������ ��ȯ ���� ���
		IS_END
	};

	enum MeleeAttacks
	{
		MA_ATTACK03, 
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
		EBONE_END
	};

	// ���� ����
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

	// �̻��� ����
	enum Missiles
	{
		MISS_ATTACK_03,
		MISS_ATTACK_05,
		MISS_ATTACK_08,
		MISS_END
	};

private:
	CM_Crownless_P2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Crownless_P2(const CM_Crownless_P2& rhs);
	virtual ~CM_Crownless_P2() = default;

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

public: // StateKey ���� �Լ� ����
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID, _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);

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
	MONINFO			m_tMonsterInfo;
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

	// MoveCollider �浹 �� ���� ����
	// �и��� �Ÿ� = ��ģ �Ÿ� * (1 - �� ���� / (��� ���� + �� ����))
	_float				m_fPushWeight = {};

	//
	_float3				m_vLevelCenterPosition = {};
	_uint				m_iTauntStack = { 0 };
	_float				m_fHitPoint = {};

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

	void Init_AttackInfos();
	void Init_Missiles();
	void Init_AnimSystem();

	// ���� ���� ��Ÿ�� TimeDelta ��ŭ �ٿ��ִ� �Լ�
	void Apply_CoolTime(_double TimeDelta);
	// ���� �ൿ(����) ����
	void Select_State(_double TimeDelta);
	// ���¿� ���� �ൿ ����
	void Tick_State(_double TimeDelta);
	// ������ ���� ���� ó��
	void On_Cell();
	// 
	void On_Hit(CGameObject* pGameObject, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos);

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CM_Crownless_P2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }

	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }

	void CM_Crownless_P2::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P2::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CM_Crownless_P2::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	class CCameraMovement* m_pCamMovement = nullptr;
};

END
