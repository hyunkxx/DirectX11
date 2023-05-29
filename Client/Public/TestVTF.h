#pragma once
#include "Client_Defines.h"
#include "TestChar.h"
#include "Renderer.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CModel_VTF;
class CSphereCollider;
class CBone;
END

BEGIN(Client)

class CTestVTF : public CTestChar
{
public:
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

public:
	struct TESTVTF_DESC
	{
		_uint iModelID;
		_uint iAnimSetID[ANIMSET_END];
		_uint iPartsID[PARTS_END];
		_tchar szFilePath[MAX_PATH];
	};

private:
	CTestVTF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestVTF(const CTestVTF& rhs);
	virtual ~CTestVTF() = default;

public:
	virtual HRESULT	Initialize_Prototype(TESTVTF_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

	CModel_Anim*	Get_Action(_uint iType)
	{
		return m_pAnimSetCom[iType];
	}

	_float*  Get_TrackPos(_uint iType)
	{
		return &m_TrackPos[iType];
	}

	MULTISTATE_TOOL* Get_CurState()
	{
		return &m_tStates[m_iStateID];
	}

	const _tchar* Get_FilePath()
	{
		return m_tDesc.szFilePath;
	}

	int* Get_StateID()
	{
		return &m_iStateID;
	}

	void Set_TrackPos(_uint iType);

	void Safe_StateParams();

	void Safe_AnimID();
	void SetUp_State();
	void SetUp_Animation();
	void SetUp_Animation(_uint iType);

	virtual const char* Get_StateTag(_uint iIndex)
	{
		if (iIndex <= SS_END)
			return CTestChar::szSharedStateTag[iIndex];
		else if (iIndex < 200)
		{
			_itoa_s((_int)iIndex, szStateTag, 10);
			return szStateTag;
		}
		else
			return nullptr;
	}

public: // StateKey ���� �Լ� ����
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID , _uint iEffectID, _bool bTracking);

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	// ������Ʈ ������Ÿ�� �б��
	TESTVTF_DESC m_tDesc;

	// ���� ������Ʈ ID ǥ�ÿ�
	char				szStateTag[10];

	// State
	MULTISTATE_TOOL		m_tStates[200];
	_float				m_TrackPos[ANIMSET_END] = { 0.f, };

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
	void Init_AnimSystem();

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
	static CTestVTF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TESTVTF_DESC* pDesc);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;



	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CTestVTF::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CTestVTF::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CTestVTF::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;
	CGameObject* pStaticObject = nullptr;

	_float m_fTime = 0.f;
	_int m_iMesh = 0;
};

END