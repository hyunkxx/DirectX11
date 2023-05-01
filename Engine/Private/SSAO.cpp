#include "..\Public\SSAO.h"
#include "GameInstance.h"
#include "TargetManager.h"
#include "RenderTarget.h"

CSSAO::CSSAO(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pBuffer = CVIBuffer_Rect::Create(pDevice, pContext);
	m_pShader = CShader::Create(pDevice, pContext, TEXT("../../Shader/SHADER_SSAO.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);

	assert(m_pDevice);
	assert(m_pContext);
	assert(m_pBuffer);
	assert(m_pShader);
}

CSSAO::~CSSAO()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CSSAO::SetBufferSize(_int iPosX, _int iPosY, _int iWidth, _int iHeight)
{
	m_fWidth = (_float)iWidth;
	m_fHeight = (_float)iHeight;
	m_fX = (_float)(iWidth >> 1);
	m_fY = (_float)(iHeight >> 1);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - m_fWidth * 0.5f, -m_fY + m_fHeight * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fWidth, (_float)m_fHeight, 0.f, 1.f));
}

void CSSAO::Apply()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Begin(0)))
		return;
	if (FAILED(m_pBuffer->Render()))
		return;
}
