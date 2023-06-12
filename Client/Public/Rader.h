#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CSphereCollider;
class CEffect;
class CTransform;
END

BEGIN(Client)

class CRader :
	public CGameObject
{
protected:
	CRader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRader(const CRader& rhs);
	virtual ~CRader() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Play_Rader(CTransform* pTransform);

private:
	void Timer(_double TimeDelta);

protected:
	CCollider* m_pCollider = { nullptr };
	CEffect*	m_pEffect = nullptr;

	_float4x4 m_WorldMatrix;

private:
	_float		m_fCoolTime = { 0.f };
	_bool		m_bReady = { true };

protected:
	HRESULT Add_Components();

public:
	static CRader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CRader::OnCollisionEnter(CCollider * src, CCollider * dest);

};

END