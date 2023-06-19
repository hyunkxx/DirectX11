#pragma once
#include "Character.h"
#include "PlayerState.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel_Anim;
class CModel_VTF;
class CTransform;
class CBone;
END

BEGIN(Client)

class CPhaseChanger : public CCharacter
{
public:
	enum STATE 
	{
		STATE_WAIT,
		STATE_1_READY,
		STATE_1_ONGOING,
		STATE_2_READY,
		STATE_2_ONGOING,
		STATE_END
	};

	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		EBONE_HEAD,
		// 여기까지 몬스터 공통으로 고정
		EBONE_END
	};

private:
	CPhaseChanger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhaseChanger(const CPhaseChanger& rhs);
	virtual ~CPhaseChanger() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	
	virtual void CutScene1_Ready();
	virtual void CutScene1_Start();
	virtual void CutScene1_End();

	virtual void CutScene2_Ready();
	virtual void CutScene2_Start();
	virtual void CutScene2_End();

	void Set_Phase1(CCharacter* pChar)
	{
		m_pCrownless_P1 = pChar;
	}
	void Set_Phase2(CCharacter* pChar)
	{
		m_pCrownless_P2 = pChar;
	}
	void Set_Phase3(CCharacter* pChar)
	{
		m_pCrownless_P3 = pChar;
	}

	HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Release_States();

	HRESULT	Init_EffectBones();
	void	Update_EffectBones();
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint iEffectBoneID, _uint iTypeID, _bool bTracking);
	void StartRim() { m_bRim = true; };

private:
	CRenderer*		m_pRendererCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

	CModel_Anim*	m_pCrownlessModel = { nullptr };
	CModel_VTF*		m_pRoverModel = { nullptr };
	CTransform*		m_pCrownlessTransform = { nullptr };
	CTransform*		m_pRoverTransform = { nullptr };

	//
	CCharacter*		m_pCrownless_P1 = { nullptr };
	CCharacter*		m_pCrownless_P2 = { nullptr };
	CCharacter*		m_pCrownless_P3 = { nullptr };
	//Playercharacter는 PlayerState에서 실시간으로 포인터 받아서 처리
	CPlayerState*	m_pPlayerStateClass = { nullptr };

	CCharacter::SINGLESTATE m_tStates[2];

	//
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};
		

	//
	_uint			m_eState = { STATE_WAIT };
	_bool			m_bRender = { false };
	_double			m_Timer = { 0.0 };

	_double			m_TrackPos = { 0.0 };
	_bool			m_bAnimFinished = { false };

private:
	class CCameraMovement* m_pCamMovement = { nullptr };
	_bool m_bZoomIn = false;
	_float m_fZoomAcc = 0.f;
	_float m_fRimAcc = 0.f;
	_bool m_bRim = false;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CPhaseChanger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	// CCharacter을(를) 통해 상속됨
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override {}
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override {}
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override {}
};

END

