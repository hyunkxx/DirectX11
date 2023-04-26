#pragma once

#include "Base.h"

BEGIN(Engine)

class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
private:
	explicit CLightManager() = default;
	virtual ~CLightManager() = default;

public:
	enum LIGHT_MATRIX { LIGHT_VIEW, LIGHT_PROJ, LIGHT_MATRIX_END };

public:
	HRESULT AddLight(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	const LIGHT_DESC* GetLightDesc(_uint Index);

	void ShadowUpdate();
	void SetLightMatrix(_fmatrix LightMatrix, LIGHT_MATRIX eLightMatrix);
	_float4x4 GetLightFloat4x4(LIGHT_MATRIX eLightMatrix);
	_float4x4 GetLightInverseFloat4x4(LIGHT_MATRIX eLightMatrix);

	void SetLightPosition(_fvector vLightPos);
	_float4 GetLightPosition() const { return m_vLightPos; };
	void SetLightDirection(_fvector vLightDir);
	_float4 GetLightDirection() const { return m_vLightDir; };

public:
	virtual void Free() override;

private:
	list<class CLight*> m_Lights;
	typedef list<class CLight*> LIGHTS;

	_float4 m_vLightPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLightDir = { 0.f, 0.f, 0.f, 0.f };
	_float4x4 m_LightMatrix[LIGHT_MATRIX_END];

};

END