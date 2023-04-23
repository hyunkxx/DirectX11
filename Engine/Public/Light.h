#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	explicit CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* GetLightDesc() const { return &m_LightDesc; }

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);


public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void Free() override;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	LIGHT_DESC m_LightDesc;

};

END