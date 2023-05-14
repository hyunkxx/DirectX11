#pragma once

#include "GameObject.h"
#include "Collider.h"

BEGIN(Client)

class CInteractionObject abstract
	: public CGameObject
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
protected:
	explicit CInteractionObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CInteractionObject(const CInteractionObject& rhs);
	virtual ~CInteractionObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() = 0;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();
	
	virtual void Free() override;

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) = 0;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) = 0;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) = 0;

};

END