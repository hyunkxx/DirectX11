#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum COLL_TYPE { COLL_SPHERE, COLL_AABB, COLL_OBB, COLL_END };

	typedef struct tagColliderDesc
	{
		class CGameObject* owner;
		_float3 vCenter;
		_float3 vExtents;
		_float3 vRotation;
	}COLLIDER_DESC;

protected:
	explicit CCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit CCollider(const CCollider& rhs);

public:
	virtual ~CCollider() = default;

public:
	virtual HRESULT InitializePrototype(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual HRESULT Initialize(void* arg) = 0;

public:
	virtual void Update(_matrix transformMatrix) = 0;
	virtual _bool Collision(CCollider* targetCollider) = 0;
	virtual HRESULT Render() { return S_OK; };

public:
	void SetActive(_bool value);
	_bool IsActive() { return m_isActive; }
	_bool IsVisible() { return m_isVisible; }
	void SetVisible(_bool value) { m_isVisible = value; }
	_bool Compare(CCollider* collider) { return this == collider; };
	void HitColliderReset() { hitCollider.clear(); }
	void EraseHitCollider(CCollider* collider);
	void AddHitCollider(CCollider* collider);
	_bool IsHitCollider(CCollider* collider);
	void SetColor(_float4 color) { m_vColor = color; };

	void SetSameObjectDetection(_bool value) { m_bSameObjectDetection = value; }
	_bool SameObjectDetection() const { return m_bSameObjectDetection; }

	void SetOwner(class CGameObject* owner) { CollDesc.owner = owner; }
	class CGameObject* GetOwner() const { return CollDesc.owner; }
	COLL_TYPE GetType() const {	return m_eCollType; }
	void SetCollision(_bool value) { m_isColl = value; }
	_bool IsColl() const { return m_isColl; }
	virtual _float3 GetCenter() const { return CollDesc.vCenter; }
	virtual _float3 GetRotation() const { return CollDesc.vRotation; }
	virtual _float3 GetExtents() const { return CollDesc.vExtents; }
	virtual void SetExtents(_float3 vExtents) {};
	virtual void SetRotation(_float3 vRotation) {};

	virtual _float3* GetWorldCenter() const { return nullptr; }


	virtual void SetCenter_(_float3 vPosition) {}
	virtual void SetExtents_(_float3 vExtents) {}

public:
	virtual CComponent* Clone(void* arg = nullptr) = 0;
	virtual void Free() override;

public:

protected:
	_bool m_bSameObjectDetection = true;
	_bool m_isActive = true;
	_bool m_isVisible = true;
	_bool m_isColl = false;

	COLLIDER_DESC CollDesc;
	COLL_TYPE m_eCollType = COLL_TYPE::COLL_END;
	
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };

	list<CCollider*> hitCollider;
protected:
	ID3D11InputLayout* _inputLayout = nullptr;
	PrimitiveBatch<VertexPositionColor>* _batch = nullptr;
	BasicEffect* _effect = nullptr;

};

class ENGINE_DLL IOnCollisionEnter
{
public:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) = 0;
};

class ENGINE_DLL IOnCollisionStay
{
public:
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) = 0;
};

class ENGINE_DLL IOnCollisionExit
{
public:
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) = 0;
};

END