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

	// 무기 거는 본 배열 인덱스
	enum PARTSBONE
	{
		PBONE_WEAPON1, // 무기 사용 중 칼집 위치
		PBONE_WEAPON2, // 무기 사용 중 검 위치
		PBONE_WEAPON3, // 무기 수납 중 칼집 위치
		PBONE_WEAPON4, // 무기 수납 중 검 위치
		PBONE_WEAPON5, // 바닥(왼쪽)?
		PBONE_WEAPON6, // 바닥(오른쪽)?
		PBONE_WEAPON7, // 등(가장 멀리 // 세로?)?
		PBONE_HULU,		// 항아리 위치
		PBONE_END
	};

	// 이펙트용 본 월드 행렬 배열 인덱스 
	// 각 본마다 현재 Tracking Effect가 실행중인지 파악하고 사용 중인 이펙트에게 전달할 행렬을 갱신함
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE2,	// 명치
		EBONE_WEAPON01,	// 왼손 무기뼈 
		EBONE_WEAPON02,	// 오른손 무기뼈
		EBONE_LHAND,	// 왼손뼈 
		EBONE_RHAND,	// 오른손뼈
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

public: // StateKey 대응 함수 모음
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID , _uint iEffectID, _bool bTracking);

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	// 오브젝트 프로토타입 분기용
	TESTVTF_DESC m_tDesc;

	// 현재 스테이트 ID 표시용
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
	// 매 프레임 이펙트 본 사용중인지 체크해서 저장, 사용중인 본만 행렬 갱신해줌
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