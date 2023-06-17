#pragma once

#include "InteractionObject.h"


BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CModel;
class CVIBuffer_Rect;
class CEffect;
END

BEGIN(Client)

class  CInvisible_Chest final : public CInteractionObject
{

public:
	enum CHEST_STATE { STATE_INVISIBLE , STATE_APPEAR , STATE_VISIVLE , STATE_END };
	enum CHEST_TYPE { CHEST_SIMPLE, CHEST_STANDARD, CHEST_EXPANDED };

private:
	explicit CInvisible_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInvisible_Chest(const CInvisible_Chest& rhs);
	virtual ~CInvisible_Chest() = default;

public:
	HRESULT Initialize_Prototype(CHEST_TYPE eChestType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

public:
	void Invisible_Tick(_double TimeDelta);
	void Appear_Tick(_double TimeDelta);
	void Visivle_Tick(_double TimeDelta);

	void Invisible_LateTick(_double TimeDelta);
	void Appear_LateTick(_double TimeDelta);
	void Visivle_LateTick(_double TimeDelta);
	void Set_Position(_float3 vPos);
public:
	virtual void Interaction(void* pArg = nullptr);

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResource();
	HRESULT SetupShadowResource();

	void interactionUIActive(_bool bRender);

public:
	static CInvisible_Chest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CHEST_TYPE eChestType);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

	void		 Off_Detection(void);
	void		 Appear_Box(void);

public:
	CCollider* GetCollider() const { return m_pCollider; };

private:
	CRenderer* m_pRenderer = nullptr;
	CCollider* m_pCollider = nullptr;
	CShader* m_pShader = nullptr;
	CModel* m_pModel = nullptr;

	CTexture* m_pEmissive = nullptr;
	CTexture* m_pSpecular = nullptr;
	CTexture* m_pSSAO = nullptr;

	CEffect* m_Invisible_Effect = nullptr;

	class CInventory* m_pInven = nullptr;
private:
	CHEST_TYPE m_eChestType = CHEST_SIMPLE;
	_bool m_bOverlapedPlayer = false;

	_float m_fTimeAcc = 0.f;

	_bool m_bInteractionUIRender = false;
	_bool m_bInteractionActive = false;
	_bool m_bInteractionBegin = false;

	_float m_fPushButtonAcc = 0.f;
	_float m_fResetTimeAcc = 0.f;

	_float3 m_vColor = { 1.f, 1.f, 1.f };

	_bool  m_bRimLight = { false };
	_float m_fRimTime = { 0.f };
	_float m_fBurstRim = { 0.3f };

	//상자 상태
	CHEST_STATE m_ChestState = { STATE_INVISIBLE };
	_bool		m_bRader_Detection = { true };
};

END