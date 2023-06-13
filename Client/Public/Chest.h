#pragma once

#include "InteractionObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CModel;

class CVIBuffer_Rect;

END

BEGIN(Client)

class CChest final : 
	public CInteractionObject
{
public:
	enum CHEST_TYPE { CHEST_SIMPLE, CHEST_STANDARD, CHEST_EXPANDED };

private:
	explicit CChest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CChest(const CChest& rhs);
	virtual ~CChest() = default;

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
	virtual void Interaction(void* pArg = nullptr);

private:
	HRESULT addComponents();
	HRESULT setupShaderResource();
	HRESULT setupShadowResource();

	void interactionUIActive(_bool bRender);

public:
	static CChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CHEST_TYPE eChestType);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

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

	class CInventory* m_pInven = nullptr;
	class CUI_Minimap* m_pUIIcon = nullptr;
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

	//UI Ãß°¡
	_int	m_UIIndex = { 0 };
};

END