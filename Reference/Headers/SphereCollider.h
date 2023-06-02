#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CSphereCollider final : public CCollider
{
protected:
	explicit CSphereCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT InitializePrototype(ID3D11Device* device, ID3D11DeviceContext* context) override;
	virtual HRESULT Initialize(void* arg = nullptr) override;

	virtual void Update(_matrix transformMatrix) override;
	virtual _bool Collision(CCollider* targetCollider) override;
	virtual HRESULT Render() override;

	virtual _float3 GetCenter() const { return CollDesc.vCenter; }
	virtual _float3* GetWorldCenter() const { return &_sphere->Center; }
	virtual _float3 GetRotation() const { return CollDesc.vRotation; }
	virtual _float3 GetExtents() const { return CollDesc.vExtents; }
	virtual void SetExtents(_float3 vExtents);
	virtual void SetRotation(_float3 vRotation);

	virtual void SetCenter_(_float3 vPosition)
	{
		_sphereOriginal->Center = vPosition;
	}
	virtual void SetExtents_(_float3 vExtents)
	{
		_sphereOriginal->Radius = vExtents.x;
	}

public:
	static CSphereCollider* Create(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual CComponent* Clone(void* arg) override;
	virtual void Free() override;
	
public:
	BoundingSphere* GetSphere() const { return _sphere; }

public:
	BoundingSphere* _sphere = nullptr;
	BoundingSphere* _sphereOriginal = nullptr;

};

END