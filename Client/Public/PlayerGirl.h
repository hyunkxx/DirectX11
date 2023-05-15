#pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "Collider.h"

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

class CPlayerGirl 
	: public CCharacter
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
	
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
		IS_START = 145,
		IS_ATTACK_01 = 145,
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

private:
	CPlayerGirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerGirl(const CPlayerGirl& rhs);
	virtual ~CPlayerGirl() = default;

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
	static const _int iState_End;

public: // StateKey ���� �Լ� ����
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID , _uint iEffectTypeID, _bool bTracking);

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	// State
	static MULTISTATE	m_tStates[SS_END + (IS_END - IS_START)];
	MULTISTATE			m_tCurState;
	_double				m_StateCoolTimes[SS_END + (IS_END - IS_START)] = { 0.0, };

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

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
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
	static CPlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CPlayerGirl::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CPlayerGirl::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CPlayerGirl::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;
	CGameObject* pStaticObject = nullptr;
	
};

END