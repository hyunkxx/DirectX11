 #pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CModel_VTF;
class CSphereCollider;
class CBone;
class CVIBuffer_Rect;
class CTexture;
class CNavigation;

END

BEGIN(Client)

class CP_PlayerGirl 
	: public CCharacter	
{
public:
	typedef enum INPUT_STATE {
		INPUT_NONE,
		INPUT_MOVE,
		INPUT_DASH,
		INPUT_SPACE,
		INPUT_TOOL,
		INPUT_ATTACK,
		INPUT_ATTACK_CHARGE,
		INPUT_ATTACK_RELEASE,
		INPUT_SKILL,
		INPUT_BURST,
		INPUT_END
	}INPUT;

	enum IndividualStates
	{
		// ĳ���ͺ� ���� �׼� = ���� �׼� ����ȣ���� ����
		IS_START = 160,
		IS_ATTACK_01 = 160,
		IS_ATTACK_02,
		IS_ATTACK_03,
		IS_ATTACK_04, // 4Ÿ ���� ������
		IS_ATTACK_05, // ��ų2 �ߵ� �� �߰�Ÿ
		IS_ATTACK_09, // ���� ����
		IS_ATTACK_PO_2, // ����Ʈ ȸ�� �� ���� 1Ÿ
		IS_ATTACK_PO_3, // ����Ʈ ȸ�� �� ���� 2Ÿ
		IS_AIRATTACK_START,
		IS_AIRATTACK_LOOP,
		IS_AIRATTACK_END,
		IS_SKILL_01,
		IS_SKILL_02,
		IS_SKILL_QTE,
		IS_BURST,
		IS_END
	};

	enum PARTS
	{
		PARTS_WEAPON_MAIN,
		PARTS_WEAPON_SUB,
		PARTS_HULU,
		PARTS_END
	};

	// ������ �� �迭 �ε���
	enum PARTSBONE
	{
		PBONE_WEAPON1, // ���� ��� �� Į�� ��ġ
		PBONE_WEAPON2, // ���� ��� �� �� ��ġ
		PBONE_WEAPON3, // ���� ���� �� Į�� ��ġ
		PBONE_WEAPON4, // ���� ���� �� �� ��ġ
		PBONE_WEAPON5, // �ٴ�(����)?
		PBONE_WEAPON6, // �ٴ�(������)?
		PBONE_WEAPON7, // ��(���� �ָ� // ����?)?
		PBONE_HULU,		// �׾Ƹ� ��ġ
		PBONE_END
	};

	// ����Ʈ�� �� ���� ��� �迭 �ε��� 
	// �� ������ ���� Tracking Effect�� ���������� �ľ��ϰ� ��� ���� ����Ʈ���� ������ ����� ������
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE2,	// ��ġ
		EBONE_WEAPON01,	// �޼� ����� 
		EBONE_WEAPON02,	// ������ �����
		EBONE_LHAND,	// �޼ջ� 
		EBONE_RHAND,	// �����ջ�
		EBONE_END
	};

	// ��Ÿ��� ��
	enum ClimbBone
	{
		CBONE_HEAD,
		CBONE_SPINE,
		CBONE_ROOT,
		CBONE_END
	};

	// ���� ����
	enum Attacks
	{
		ATK_NONE,		// 0�� ����ó�������� NONE���� �־���� ��
		ATK_ATTACK_01,
		ATK_ATTACK_02,
		ATK_ATTACK_03,
		ATK_ATTACK_04, 
		ATK_ATTACK_05_01, // ��ų2 �߰�Ÿ �� 1Ÿ 
		ATK_ATTACK_05_02, // ��ų2 �߰�Ÿ �� 2Ÿ
		ATK_ATTACK_09,	// ���� ���� ����ü �ٴ� ��Ʈ
		ATK_ATTACK_PO_2, // ����Ʈ ȸ�� �� ���� 1Ÿ
		ATK_ATTACK_PO_3, // ����Ʈ ȸ�� �� ���� 2Ÿ
		ATK_AIRATTACK,
		ATK_SKILL_01,
		ATK_SKILL_02_01, // ��ų2 1Ÿ
		ATK_SKILL_02_02, // ��ų2 2Ÿ
		ATK_SKILL_02_03, // ��ų2 �̻���
		ATK_SKILL_QTE,	// ��ü QTE
		ATK_BURST,		// �ʻ��
		ATK_END
	};

private:
	CP_PlayerGirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CP_PlayerGirl(const CP_PlayerGirl& rhs);
	virtual ~CP_PlayerGirl() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

	// static
	static HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static void Release_States();
	static const char szIndividualStateTag[IS_END - IS_START][MAX_PATH];
	static const _int iStateLimit;

	// 
	virtual void Check_Nearst(CCharacter* pChar, _float fDist) override;

	// �ֺ� ���� ����Ʈ
	// ���� �ʿ��� �÷��̾���� �Ÿ��� ������ ������ ��? �÷��̾ �߰����� �� ����
	// 
	
public: // StateKey ���� �Լ� ����
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID , _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);

public:
	virtual _uint Get_AttackID() override { return m_iCurAttackID; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* fAttackOut) override
	{
		memcpy(pAttackInfoOut, &m_AttackInfos[iAttackID], sizeof(TAGATTACK));
		*fAttackOut = m_tCharInfo.fAttack;
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	// State
	static MULTISTATE	m_tStates[CP_PlayerGirl::IS_END];
	MULTISTATE			m_tCurState;
	_double				m_StateCoolTimes[CP_PlayerGirl::IS_END] = { 0.0, };

	// Parts 
	class CParts*		m_Parts[PARTS_END] = { nullptr, };
	CBone*				m_PartsBone[PBONE_END] = { nullptr, };

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};

	// �� ������ ����Ʈ �� ��������� üũ�ؼ� ����, ������� ���� ��� ��������
	_bool				m_bEffectBoneActive[EBONE_END] = { false, };

	// ��Ÿ��� ��
	CBone*				m_pClimbBones[CBONE_END] = { nullptr };
	CBone*				m_pHeadBone = { nullptr };
	_float3				m_vClimbExitPos = {};
	_float				m_fClimbExitYGap = {};

	// ���� ����ü
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };

	// �÷��̾� ����
	// ���� ���� ���� Ƚ��
	_uint				m_iAirJumpCount = { 100 };
	// ���� ��ġ ��� - ���� ����ó��
	_float3				m_fJumpPos = {};
	// ��ų ��ȭ ���� üũ
	_float				m_fSkillGauge = { 100.f };

	_double				m_ChargeAcc = { 0.0 };

	// ����Ʈ ����� �ӽ�
	_float4x4			m_WorldMatrix;

	_bool				m_bInputLock = { false };

	//
	CCharacter*			m_pNearst = { nullptr };
	_float				m_fNearstDist = { 0.f };
	CCharacter*			m_pFixedTarget = { nullptr };
	
	// MoveCollider �浹 �� ���� ����
	// �и��� �Ÿ� = ��ģ �Ÿ� * (1 - �� ���� / (��� ���� + �� ����))
	_float				m_fPushWeight = { 0.f };

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void Init_AttackInfos();
	void SetUp_State();
	// bContinue == �ܿ� �������� ����ϴ� �ִϸ��̼�����?
	void SetUp_Animations(_bool bContinue);

	// ���� �ൿ(����) ����
	void Key_Input(_double TimeDelta);
	// ���¿� ���� �ൿ ����
	void Tick_State(_double TimeDelta);
	// ������ ���� ���� ó��
	void On_Cell();
	
	// Parts
	HRESULT Init_Parts();

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

private:
	void Set_WeaponUse(_bool bBool);

public:
	static CP_PlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	CCollider* GetDodgeCollider() const { return m_pDodgeCollider; }
	void CP_PlayerGirl::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	CCollider* m_pDodgeCollider = nullptr;

	CGameObject* pStaticObject = nullptr;
};

END
