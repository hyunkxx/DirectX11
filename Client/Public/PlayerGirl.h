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
		return m_pActionCom[iType];
	}

	CModel_Anim*	Get_Pose(_uint iType)
	{
		return m_pPoseCom[iType];
	}

	_float*  Get_TrackPos(_uint iType)
	{
		return &m_TrackPos[iType];
	}

	MULTISTATE* Get_CurState()
	{
		return &m_tState[m_iStateID];
	}

	int* Get_StateID()
	{
		return &m_iStateID;
	}

	void Set_TrackPos(_uint iType);

	void Safe_StateID();

	void Safe_PoseID();
	void SetUp_Pose();

	void Safe_AnimID();
	void SetUp_Animation();
	void SetUp_Animation(_uint iType);

	static const char szIndividualStateTag[IS_END - IS_START][MAX_PATH];
	static const _int iState_End;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pPoseCom[ANIMSET_END] = { nullptr };
	CModel_Anim*		m_pActionCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	MULTISTATE			m_tState[SS_END + (IS_END - IS_START)];
	_float				m_TrackPos[ANIMSET_END] = { 0.0, };

private:
	HRESULT Add_Components();
	HRESULT Init_States();
	void Tick_State(_double TimeDelta);
	void Init_AnimSystem();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CPlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
};

END