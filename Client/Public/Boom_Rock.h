#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CSphereCollider;
class CTransform;
class CModel;
END

BEGIN(Client)
//260 38 227
class CBoom_Rock :
	public CGameObject
	, public IOnCollisionEnter
{
private:
	CBoom_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoom_Rock(const CBoom_Rock& rhs);
	virtual ~CBoom_Rock() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	CCollider* m_pCollider = { nullptr };
	CCollider* m_pCollider_Lader = { nullptr };
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

	_float4x4 m_WorldMatrix;

	_bool	  m_bRender = { true };

	_bool  m_bRimLight = { false };
	_float m_fRimTime = { 0.f };
	_float m_fBurstRim = { 0.3f };


protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResource();

public:
	static CBoom_Rock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CBoom_Rock::OnCollisionEnter(CCollider * src, CCollider * dest)override;
};

END