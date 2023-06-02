#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBBCollider final : public CCollider
{
protected:
	explicit COBBCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit COBBCollider(const COBBCollider& rhs);
	virtual ~COBBCollider() = default;

public:
	virtual HRESULT InitializePrototype(ID3D11Device* device, ID3D11DeviceContext* context) override;
	virtual HRESULT Initialize(void* arg = nullptr) override;

	virtual void Update(_matrix transformMatrix) override;
	virtual _bool Collision(CCollider* targetCollider) override;
	virtual HRESULT Render() override;

	virtual _float3 GetCenter() const { return CollDesc.vCenter; }
	virtual _float3* GetWorldCenter() const { return &_obb->Center; }
	virtual _float3 GetRotation() const { return CollDesc.vRotation; }
	virtual _float3 GetExtents() const { return CollDesc.vExtents; }
	virtual void SetExtents(_float3 vExtents);
	virtual void SetRotation(_float3 vRotation);

	_float3 GetCorners();

	virtual void SetCenter_(_float3 vPosition)
	{
		_obbOriginal->Center = vPosition;
	}
	virtual void SetExtents_(_float3 vExtents) 
	{
		_obbOriginal->Extents = vExtents;
	}

public:
	static COBBCollider* Create(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual CComponent* Clone(void* arg) override;
	virtual void Free() override;

public:
	BoundingOrientedBox* GetObb() const { return _obb; }

public:
	BoundingOrientedBox* _obb = nullptr;
	BoundingOrientedBox* _obbOriginal = nullptr;

};

END