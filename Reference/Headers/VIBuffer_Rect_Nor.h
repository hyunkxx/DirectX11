#pragma once
#include "VIBuffer.h"

//CVIBuffer_Rect_Nor


BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Nor final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect_Nor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Nor(const CVIBuffer_Rect_Nor& rhs);
	virtual ~CVIBuffer_Rect_Nor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Rect_Nor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END