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
END

BEGIN(Client)

class CPlayerGirl : public CCharacter
{
public:

	typedef enum INPUT_STATE {
		INPUT_NONE,
		INPUT_MOVE,
		INPUT_DASH,
		INPUT_SPACE,
		INPUT_ATTACK,
		INPUT_ATTACK_CHARGE,
		INPUT_ATTACK_RELEASE,
		INPUT_ELMT_ART,
		INPUT_ELMT_BURST,
		INPUT_END
	}INPUT;


	enum IndividualStates
	{
		// ĳ���ͺ� ���� �׼� = ���� �׼� ����ȣ���� ����
		IS_START = 145,
		IS_ATTACK_01 = 145,
		IS_ATTACK_02,
		IS_ATTACK_03,
		IS_ATTACK_04,
		IS_ATTACK_05,
		IS_ATTACK_09,
		IS_ATTACK_PO_2,
		IS_ATTACK_PO_3,
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

	// ���� �Ŵ� �� �迭 �ε���
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

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	// State
	static MULTISTATE	m_tStates[SS_END + (IS_END - IS_START)];
	MULTISTATE			m_tCurState;
	_double				m_StateCoolTimes[SS_END + (IS_END - IS_START)] = { 0.0, };

	// Parts
	class CParts*		m_Parts[PARTS_END] = { nullptr, };
	CBone*				m_PartsBone[PBONE_END] = { nullptr, };

	// ����Ʈ ����� �ӽ�
	_float4x4			m_WorldMatrix;

	_bool				m_bInputLock = { false };

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void SetUp_State();
	void SetUp_Animations(_bool bContinue);

	void Key_Input(_double TimeDelta);
	void Tick_State(_double TimeDelta);
	
	// Parts
	HRESULT Init_Parts();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

private:
	void Set_WeaponUse(_bool bBool);

public:
	static CPlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
	
};

END