#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator final : public CComponent
{

private:
	CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCalculator(const CCalculator& rhs);
	virtual ~CCalculator() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	_float3 Picking_OnTerrain(
		HWND hWnd,
		class CVIBuffer_Terrain* pVerticesTerrainCom,
		class CTransform* pTransformCom
	);

private:
	_float3						m_vMousePicking_Positon = { 0.f, 0.f, 0.f };

public:
	static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END