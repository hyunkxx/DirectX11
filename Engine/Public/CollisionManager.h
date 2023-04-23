#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionManager final : public CBase
{
	DECLARE_SINGLETON(CCollisionManager)
public:
	CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	HRESULT ReserveCollisionLayer(_uint iLayerCount);
	_bool IsDebugRender() const { return m_bDebugRender; }
	void SetDebugRender(_bool value) { m_bDebugRender = value; }

public:
	HRESULT AddCollider(class CCollider* collider, _uint iLayerIndex = 0);
	void PhysicsUpdate();
	void Render();
	int GetHasCollisionCount(_uint iLayerIndex);

private:
	void Clear();

public:
	virtual void Free() override;

private:
	_bool m_bDebugRender = true;
	_uint m_iCollisionsCount = 0;
	vector<class CCollider*>* m_Collisions = nullptr;

};

END