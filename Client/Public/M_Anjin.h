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
class CM_Anjin
	: public CCharacter
{
public:
	enum AI_STATE
	{
		AI_NONE,
		AI_IDLE,
		AI_CHASE,
		AI_STAY,
		AI_ATTACK,
		AI_COMBO,
		AI_END
	};

	enum IndividualStates
	{
		IS_IDLE,
		IS_GREETING,
		IS_WALK_B,
		IS_WALK_F,
		IS_WALK_L,
		IS_WALK_R,   //������ �ȱ� �ִϸ��̼� ���峪�־ �Ⱦ�
		IS_RUN,
		IS_ATTACK01,
		IS_ATTACK02_1,
		IS_ATTACK02_2,
		IS_ATTACK02_3,
		IS_ATTACK03_READY,
		IS_ATTACK03,
		IS_STANDUP,
		IS_BEHIT_S, // Small
		IS_BEHIT_B, // Big
		IS_BEHIT_FLY_START, // or PUSH  ~~ ���� �����
		IS_BEHIT_FLY_LOOP, // or PUSH  ~~ ���� �����
		IS_BEHIT_FLY_FALL, // or PUSH  ~~ ���� �����
		IS_BEHIT_HOVER, // �� ä�� ����
		IS_BEHIT_PRESS, // ���� ä�� ����
		IS_BEHIT_BLOCK, // �и�����
		IS_DEAD,
		IS_END
	};

	// ����Ʈ�� �� ���� ��� �迭 �ε��� 
	// �� ������ ���� Tracking Effect�� ���������� �ľ��ϰ� ��� ���� ����Ʈ���� ������ ����� ������
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_RHAND,
		EBONE_END
	};

	// ���� ����
	enum Attacks
	{
		ATK_ATTACK_01,
		ATK_ATTACK_02_1,
		ATK_ATTACK_02_2,
		ATK_ATTACK_02_3,
		ATK_ATTACK_03,
		ATK_END
	};

	// �̻��� ����
	enum Missiles
	{
		MISS_ATTACK_01,
		MISS_ATTACK_03,
		MISS_END
	};

private:
	CM_Anjin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Anjin(const CM_Anjin& rhs);
	virtual ~CM_Anjin() = default;

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
		*pAttackOut = m_tCharInfo.fAttack;
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };
	//UI�߰�
	CGameObject*		m_pUIMon = { nullptr };

private:
	// State
	static SINGLESTATE	m_tStates[IS_END];
	SINGLESTATE			m_tCurState;
	_double				m_StateCoolTimes[IS_END] = { 0.0, };

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};
	// �� ������ ����Ʈ �� ��������� üũ�ؼ� ����, ������� ���� ��� ��������
	_bool				m_bEffectBoneActive[EBONE_END] = { false, };


	// ���� ����ü
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB ��Ʈ �� ����� ���� ����üID

												// �̻��� Ǯ
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr, };
	_float3				m_MissileRotAngles[MISS_END];



	// ���� ����
	// Ÿ�� �÷��̾� > �����Ǵ� Ÿ�ֿ̹� ������ �÷��̾� ����
	CCharacter*			m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	// �÷��̾� ���� ����
	_bool				m_bAlert = { false };
	// ������ �÷��̾� ���� �Ÿ�
	_float				m_fAlertRange = { 0.f };
	// ���� ����
	_float				m_fAttackRange = { 0.f };
	// ���Ϳ� �÷��̾� ���� �Ÿ�
	_float				m_fTargetDistance = { 0.f };

	// ���� �ൿ�� ���� �۷ι� ��Ÿ�� ����
	_double				m_GlobalCoolTime = { 0.0 };
	_bool				m_bAttackReady = { false };

	// MoveCollider �浹 �� ���� ����
	// �и��� �Ÿ� = ��ģ �Ÿ� * (1 - �� ���� / (��� ���� + �� ����))
	_float				m_fPushWeight = {};

private:
	HRESULT Add_Components();
	void SetUp_State();
	// �÷��̾ ���� �Ÿ� ������ �����ߴ��� Ȯ���ϴ� �Լ�
	void Find_Target();
	// 

	void Init_AttackInfos();
	void Init_Missiles();

	// ���� ���� ��Ÿ�� TimeDelta ��ŭ �ٿ��ִ� �Լ�
	void Apply_CoolTime(_double TimeDelta);
	// ���� �ൿ(����) ����
	// void Key_Input(_double TimeDelta);
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
	static CM_Anjin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	void CM_Anjin::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CM_Anjin::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CM_Anjin::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;
};

END