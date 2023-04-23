#include "stdafx.h"
#include "..\Public\AppManager.h"

IMPLEMENT_SINGLETON(CAppManager)

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
