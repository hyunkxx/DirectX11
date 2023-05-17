#include "stdafx.h"
#include "..\Public\AppManager.h"

IMPLEMENT_SINGLETON(CAppManager)

_uint CAppManager::ConvertNumberToTexture(_uint iNumber, _uint iDigit)
{
	string strAmount = to_string(iNumber);
	size_t iLength = strAmount.size();

	assert(iDigit < iLength);

	_int iNum = strAmount[iDigit] - '0';

	return iNum;
}

_uint CAppManager::GetNumberLength(_uint iAmount)
{
	string strAmount = to_string(iAmount);
	_uint iLength = (_uint)strAmount.size();

	return iLength;
}

void CAppManager::ComputeOrtho(ORTHO_DESC* eOrtho)
{
	XMStoreFloat4x4(&eOrtho->WorldMatrix, XMMatrixScaling(eOrtho->fWidth, eOrtho->fHeight, 1.f) * XMMatrixTranslation(eOrtho->fX - g_iWinSizeX * 0.5f, -eOrtho->fY + g_iWinSizeY * 0.5f, 0.f));
}

void CAppManager::SetTitle(wstring strTitle)
{
	if (m_strTitleText != strTitle)
	{
		m_strTitleText = strTitle;
		SetWindowText(g_hWnd, m_strTitleText.c_str());
	}
}

void CAppManager::Free()
{
}
