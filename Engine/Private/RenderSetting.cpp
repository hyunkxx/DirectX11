#include "..\Public\RenderSetting.h"

IMPLEMENT_SINGLETON(CRenderSetting)

CRenderSetting::CRenderSetting()
{
	m_tagSplitDesc.m_fDistortion = 0.2f;
	m_tagSplitDesc.m_fStrength = 0.01f;
	m_tagSplitDesc.m_fSeparation = 0.1f;
}

void CRenderSetting::Free()
{
}