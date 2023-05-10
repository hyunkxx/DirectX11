#include "..\Public\RenderSetting.h"

IMPLEMENT_SINGLETON(CRenderSetting)

CRenderSetting::CRenderSetting()
{
	m_tagSplitDesc.m_fDistortion = 0.2f;
	m_tagSplitDesc.m_fStrength = 0.01f;
	m_tagSplitDesc.m_fSeparation = 0.1f;

	m_FadeDesc.m_eFadeState = FADE_STATE::FADE_NONE;
	m_FadeDesc.m_fFadeAcc = 0.f;
	m_FadeDesc.m_fFadeLimit = 1.f;
}

void CRenderSetting::Free()
{
}