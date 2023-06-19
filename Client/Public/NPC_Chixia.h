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

class CCameraMovement;
class CUI_MainScreen;
class CNPC_Chixia
	: public CCharacter
{
public:
	enum STATE
	{
		STATE_STAND1,
		STATE_STAND1_ACTION,
		STATE_END
	};

private:
	CNPC_Chixia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Chixia(const CNPC_Chixia& rhs);
	virtual ~CNPC_Chixia() = default;

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

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	MULTISTATE			m_tStates[STATE_END];

	STATE				m_eState = STATE_STAND1;

	// Rim Light Alpha
	_bool m_bRimToggle = false;
	_float m_fBurstRim = 1.f;
	_float m_fRimAlpha = 0.f;

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Release_States();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CNPC_Chixia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	void CNPC_Chixia::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CNPC_Chixia::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CNPC_Chixia::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;
	class CUI_Tip* m_UITip = { nullptr };
};

END