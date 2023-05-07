#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SSDBox final : public CVIBuffer
{
private:
	explicit CVIBuffer_SSDBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_SSDBox(const CVIBuffer_SSDBox& rhs);
	virtual ~CVIBuffer_SSDBox() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float3* GetVertexPosition() { return m_pVertexPosition; }

public:
	static CVIBuffer_SSDBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	_float3* m_pVertexPosition = { nullptr };

};

END