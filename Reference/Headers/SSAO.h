#pragma once

#include "Base.h"

BEGIN(Engine)

class CSSAO final
{
public:
	explicit CSSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSSAO();

public:
	void SetBufferSize(_int iPosX, _int iPosY, _int iWidth, _int iHeight);
	void Apply();

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CVIBuffer_Rect* m_pBuffer = nullptr;
	class CShader* m_pShader = nullptr;

private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

};

END