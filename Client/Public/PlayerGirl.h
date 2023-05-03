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
END

BEGIN(Client)

// AnimTool에서 플레이어 외의 모든 캐릭터들의 모델을 출력하고 편집하기 위한 클래스

class CPlayerGirl : public CCharacter
{
public:
	enum IndividualStates
	{
		// 캐릭터별 고유 액션 = 공용 액션 끝번호에서 시작
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

	CModel_Anim*	Get_Action(_uint iType)
	{
		return m_pAnimSetCom[iType];
	}

	_float*  Get_TrackPos(_uint iType)
	{
		return &m_TrackPos[iType];
	}

	MULTISTATE* Get_CurState()
	{
		return &m_tStates[m_iStateID];
	}

	int* Get_StateID()
	{
		return &m_iStateID;
	}

	void Set_TrackPos(_uint iType);

	void Safe_StateID();

	void Safe_AnimID();
	void SetUp_Animation();
	void SetUp_Animation(_uint iType);

	virtual const char* Get_StateTag(_uint iIndex)
	{
		if (iIndex <= SS_END)
			return CCharacter::szSharedStateTag[iIndex];
		else if (iIndex < IS_END)
			return CPlayerGirl::szIndividualStateTag[iIndex];
		else
			return nullptr;
	}

	static const char szIndividualStateTag[IS_END - IS_START][MAX_PATH];
	static const _int iState_End;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	// State
	MULTISTATE			m_tStates[SS_END + (IS_END - IS_START)];
	_float				m_TrackPos[ANIMSET_END] = { 0.f, };

	// Parts
	class CParts*		m_Parts[PARTS_END] = { nullptr, };
	CBone*				m_PartsBone[PBONE_END] = { nullptr, };

private:
	HRESULT Add_Components();
	// State
	HRESULT Init_States();
	void Tick_State(_double TimeDelta);
	void Init_AnimSystem();
	// Parts
	HRESULT Init_Parts();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CPlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
};

END