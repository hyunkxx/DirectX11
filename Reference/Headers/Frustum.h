#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
public:
	CFrustum() = default;
	virtual ~CFrustum() = default;
	virtual void Free() override;

public:
	HRESULT Initialize();
	void Tick();

public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool InWorldSpace(_fvector vPoint, _float fRadius);
	_bool InLocalSpace(_fvector vPoint, _float fRadius);

private:
	void Make_Plane(IN const _float3* pPoints, OUT _float4* pPlanes);
	
private:
	_float3	m_vOriginal_Points[8];
	_float3	m_vPoints[8];
	_float4	m_vPlane_InWorld[6];
	_float4	m_vPlane_InLocal[6];
	_float4	m_vPlane[6];

};

END