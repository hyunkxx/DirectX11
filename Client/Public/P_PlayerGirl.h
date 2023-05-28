 #pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "MissilePool.h"
#include "PlayerState.h"

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

class CCameraMovement;

class CP_PlayerGirl 
	: public CCharacter	
{
public:
	typedef enum INPUT_STATE {
		INPUT_NONE,
		INPUT_MOVE,
		INPUT_DASH,
		INPUT_FRONTSTEP,
		INPUT_BACKSTEP,
		INPUT_SPACE,
		INPUT_ATTACK,
		INPUT_ATTACK_CHARGE,
		INPUT_ATTACK_RELEASE,
		INPUT_SKILL,	// E : ĳ���� ��ų
		INPUT_BURST,	// R : ĳ���� �ʻ��
		INPUT_SUMMON,	// Q : ���� ��ȯ
		INPUT_TOOL,		// T : ���� ���
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
		ATK_NONE,		// 0�� OBB ����ó�������� NONE���� �־���� ��
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
		ATK_BURST_01,	// �ʻ��
		ATK_BURST_02,
		ATK_END
	};

	// �̻��� ����
	enum Missiles
	{
		MISS_ATTACK_03,
		MISS_ATTACK_09,
		MISS_ATTACK_PO_2, 
		MISS_ATTACK_PO_3, 
		MISS_AIRATTACK,
		MISS_SKILL_02, 
		MISS_SKILL_QTE,
		MISS_BURST_01,	// �ʻ�� 1Ÿ
		MISS_BURST_02,	// �ʻ�� 2Ÿ
		MISS_END
	};

private:
	CP_PlayerGirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CP_PlayerGirl(const CP_PlayerGirl& rhs);
	virtual ~CP_PlayerGirl() = default;

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
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);
	virtual void Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed);
	virtual void Shot_SlowKey(_float fTargetTime, _float fLerpSpeed);

public:
	virtual _uint Get_AttackID() override { return m_iCurAttackID; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* pAttackOut) override
	{
		// ��� ���
		memcpy(pAttackInfoOut, &m_AttackInfos[iAttackID], sizeof(TAGATTACK));
		// ���� ���ݷ�
		*pAttackOut = m_pCharacterState[CPlayerState::CHARACTER_ROVER].fAttack[CPlayerState::STAT_TOTAL];
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }

public:

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
	_float3				m_vClimbExitPos = {};
	_float				m_fClimbExitYGap = {};

	// ���� ����ü
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB ��Ʈ �� ����� ���� ����üID

	// �̻��� Ǯ
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr,};
	_float3				m_MissileRotAngles[MISS_END];

	// �÷��̾� ����
	// ���� ���� ���� Ƚ��
	_uint				m_iAirJumpCount = { 100 };
	// ���� ��ġ ��� - ���� ����ó��
	_float3				m_vJumpPos = {};
	// ��ų ��ȭ ���� üũ
	_float				m_fSkillGauge = { 100.f };

	_double				m_ChargeAcc = { 0.0 };

	// ����Ʈ ����� �ӽ�
	_float4x4			m_WorldMatrix;

	_bool				m_bInputLock = { true };

	// Ÿ�� ���� ������, ĳ���� ��ü �� �� ���濡 ����
	CCharacter*			m_pNearst = { nullptr };
	_float				m_fNearstDist = { 0.f };
	CCharacter*			m_pFixedTarget = { nullptr };
	_float				m_fTargetDist = { 0.f };
	_double				m_ReleaseTargetTimeAcc = { 0.0 };

	_bool				m_bInputDirMove = { false };
	_float3				m_vInputDir = { };
	
	
	// MoveCollider �浹 �� ���� ����
	// �и��� �Ÿ� = ��ģ �Ÿ� * (1 - �� ���� / (��� ���� + �� ����))
	_float				m_fPushWeight = { 0.f };

	// �ӽ� ������ ����
	_float m_fDissolveAmount = 0.f;
	_float m_fDissolveTimeAcc = 0.f;
	_bool m_bDissolve = false;
	_bool m_bDissolveType = false;
	_float m_fDissolveSpeed = 1.f;

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void Init_AttackInfos();
	void Init_Missiles();

	void SetUp_State();
	// bContinue == �ܿ� �������� ����ϴ� �ִϸ��̼�����?
	void SetUp_Animations(_bool bContinue);

	// ���� ���� ��Ÿ�� TimeDelta ��ŭ �ٿ��ִ� �Լ�
	void Apply_CoolTime(_double TimeDelta);
	// ���� �ൿ(����) ����
	void Key_Input(_double TimeDelta);
	// ���¿� ���� �ൿ ����
	void Tick_State(_double TimeDelta);
	// ������ ���� ���� ó��
	void On_Cell();
	// �ǰ� ó�� �Լ� 
	void On_Hit(CGameObject* pGameObject, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos);

	void On_Dodge();
	
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
	void CP_PlayerGirl::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	CGameObject* pStaticObject = nullptr;

	CPlayerState* m_pPlayerStateClass;
	CPlayerState::CHARACTER_STATE* m_pCharacterState;
	class CInventory* m_pInven = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;
};

END
