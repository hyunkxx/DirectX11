#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagPass
	{
		ID3D11InputLayout* pInputLayout = nullptr;
		ID3DX11EffectPass* pPass = nullptr;
	}PASS;

private:
	explicit CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iElementCount);
	virtual	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT SetMatrix(const char* pName, const _float4x4* pMatrix);
	HRESULT	Set_Matrices(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT SetRawValue(const char* pConstantName, const void* pData, _uint iSize);
	HRESULT SetShaderResourceView(const char* pName, ID3D11ShaderResourceView* pSRV);

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iElementCount);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint m_iPassCount = { 0 };
	PASS* m_pPasses = { nullptr };
	ID3DX11Effect* m_pEffect = { nullptr };

};

END