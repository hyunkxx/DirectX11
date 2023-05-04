#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar * pFontName)
{
	m_pFont = new SpriteFont(m_pDevice, pFontName);
	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

void CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRadian, const _float2& vOrigin, const _float2& vScale)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRadian, vOrigin, vScale);

	m_pBatch->End();
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontName)
{
	CCustomFont*		pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontName)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}