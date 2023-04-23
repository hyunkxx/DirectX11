#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube : public CVIBuffer
{
private:
	explicit CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Cube(const CVIBuffer_Cube& rhs);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float3* GetVertexPosition() { return m_pVertexPosition; }

public:
	static CVIBuffer_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	_float3* m_pVertexPosition = { nullptr };

};

END