#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
protected:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual	HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual	HRESULT Initialize(void* pArg);

public:
	_float Compute_Height(_fvector vPosition);

private:
	_float3*			m_pVerticesPos = { nullptr };
	_ushort*			m_pIndices = { nullptr };

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END