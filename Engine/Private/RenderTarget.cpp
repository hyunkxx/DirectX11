#include "..\Public\RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor)
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vColor = vColor;

	return S_OK;
}

HRESULT CRenderTarget::Set_ShaderResourceView(CShader * pShader, const char * pContantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	return pShader->SetShaderResourceView(pContantName, m_pSRV);
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vColor);

	return S_OK;
}

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, eFormat, vColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_fX = fX;
	m_fY = fY;
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	_uint iViewPortCount = 1;
	D3D11_VIEWPORT ViewPortDesc;
	m_pContext->RSGetViewports(&iViewPortCount, &ViewPortDesc);

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(m_fSizeX, m_fSizeY, 1.f) *
		XMMatrixTranslation(m_fX - ViewPortDesc.Width * 0.5f, -m_fY + ViewPortDesc.Height * 0.5f, 0.f));

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (FAILED(pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return S_OK;

	if (FAILED(pShader->SetShaderResourceView("g_DiffuseTexture", m_pSRV)))
		return S_OK;

	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	return pVIBuffer->Render();
}
#endif