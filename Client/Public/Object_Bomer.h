#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"
#include "Renderer.h"

BEGIN(Engine)
class CSphereCollider;
class CEffect;
class CTransform;
END

BEGIN(Client)

class CObject_Bomber :
	public CGameObject
	, public IOnCollisionEnter
{
public:
	enum STATE_ID
	{
		ID_IDLE , ID_CATCH , ID_THROW , ID_BOOM , STATE_END
	};

public:
	CObject_Bomber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Bomber(const CObject_Bomber& rhs);
	virtual ~CObject_Bomber() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	
	void State_Control(_double TimeDelta);

public:
	void Catch_Bomber(CTransform* pTransform);
	void Shoot_Bomber();

protected:
	CCollider* m_pCollider = { nullptr };
	CEffect*   m_pEffect = { nullptr };

	CTransform* m_pCatchTransform = { nullptr };
	class CPlayerState* m_pPlayerState = { nullptr };
	class CCharacter* m_pTarget = { nullptr };

	_float3	  m_vThrowLook;
	_float3	  m_vStartPos;

	_float4x4 m_StartMatrix;
	_float4x4 m_WorldMatrix;
	_float4x4 m_CatchMatrix;
	_float	  m_fSpeed = { 2.f };
	STATE_ID  m_eState = { ID_IDLE };

	_float	  m_fResetTime = { 0.f };
	_float	  m_fCatchTime = { 0.f };
	_float	  m_fThrowTime = { 0.f };

protected:
	HRESULT Add_Components();

public:
	static CObject_Bomber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const
	{ 
		return m_pCollider;
	}
	void CObject_Bomber::OnCollisionEnter(CCollider * src, CCollider * dest) override;

};

END