#pragma once

#include "InteractionObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;

END

BEGIN(Client)

class CChest final : 
	public CInteractionObject
{
private:
	explicit CChest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CChest(const CChest& rhs);
	virtual ~CChest() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

public:
	CCollider* GetCollider() const { return m_pCollider; };

private:
	HRESULT addComponents();

public:
	static CChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

private:
	CRenderer* m_pRenderer = nullptr;
	CCollider* m_pCollider = nullptr;
	CShader* m_pShader = nullptr;

private:
	_bool m_bOverlapedPlayer = false;

};

END