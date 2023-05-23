#pragma once
#include "Client_Defines.h"
#include "TestChar.h"
#include "Renderer.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CSphereCollider;
class CBone;
END

BEGIN(Client)

// AnimTool���� �÷��̾� ���� ��� ĳ���͵��� ���� ����ϰ� �����ϱ� ���� Ŭ����

class CTestGeneric : public CTestChar
{
public:
	enum PARTS
	{
		PARTS_WEAPON_MAIN,
		PARTS_WEAPON_SUB,
		PARTS_HULU,
		PARTS_END
	};

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
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		// ������� ���� �������� ����
		EBONE_END
	};

public:
	struct TESTGENERIC_DESC
	{
		_uint iModelID;
		_uint iPartsID[PARTS_END];
		_tchar szFilePath[MAX_PATH];
	};

private:
	CTestGeneric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestGeneric(const CTestGeneric& rhs);
	virtual ~CTestGeneric() = default;

public:
	virtual HRESULT	Initialize_Prototype(TESTGENERIC_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

	CModel_Anim*	Get_Action()
	{
		return m_pModelCom;
	}

	_float*  Get_TrackPos()
	{
		return &m_TrackPos;
	}

	SINGLESTATE_TOOL* Get_CurState()
	{
		return &m_tStates[m_iStateID];
	}

	int* Get_StateID()
	{
		return &m_iStateID;
	}

	const _tchar* Get_FilePath()
	{
		return m_tDesc.szFilePath;
	}

	void Set_TrackPos();

	void Safe_StateParams();

	void Safe_AnimID();
	void SetUp_State();
	void SetUp_Animation();
	void SetUp_Animation(_uint iType);

public: // StateKey ���� �Լ� ����
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID, _uint iEffectID, _bool bTracking);

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };

private:
	// ������Ʈ ������Ÿ�� �б��
	TESTGENERIC_DESC m_tDesc;

	// State
	SINGLESTATE_TOOL		m_tStates[200];
	_float				m_TrackPos = { 0.f, };

	// Parts
	class CParts*		m_Parts[PARTS_END] = { nullptr, };
	CBone*				m_PartsBone[PBONE_END] = { nullptr, };

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};
	// �� ������ ����Ʈ �� ��������� üũ�ؼ� ����, ������� ���� ��� ��������
	_bool				m_bEffectBoneActive[EBONE_END] = { false, };


	_bool m_bInputLock = { false };



private:
	HRESULT Add_Components();
	// State
	HRESULT Init_States();
	//void Init_AnimSystem();

	void Tick_State(_double TimeDelta);

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
	static CTestGeneric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TESTGENERIC_DESC* pDesc);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;



	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CTestGeneric::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CTestGeneric::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CTestGeneric::OnCollisionExit(CCollider * src, CCollider * dest) override;


	CCollider* m_pCollider = nullptr;
	CGameObject* pStaticObject = nullptr;
};

END