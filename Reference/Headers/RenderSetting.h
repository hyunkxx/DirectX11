#pragma once

#include "Base.h"
#include "Renderer.h"

BEGIN(Engine)

class CRenderSetting : public CBase
{
public:
	DECLARE_SINGLETON(CRenderSetting)

public:
	enum SHADOW_LEVEL { SHADOW_OFF, SHADOW_LOW, SHADOW_HIGH, SHADOW_END };
	enum FADE_STATE { FADE_IN, FADE_OUT, FADE_NONE };
	enum SPLIT_DIR { SPLIT_DEFAULT, SPLIT_REVERSE };

	typedef struct tagSplitRGB
	{
		_float m_fDistortion;
		_float m_fStrength;
		_float m_fSeparation;
	}RGB_SPLIT_DESC;

	typedef struct tagFadeDesc
	{
		FADE_STATE  m_eFadeState;
		_float m_fFadeAcc;
		_float m_fFadeLimit;
	}FADE_DESC;

public:
	explicit CRenderSetting();
	virtual ~CRenderSetting() = default;
	virtual void Free() override;

public:
	// DebugRender
	void DebugTargetToggle() {
		m_bDebugRenderTarget = !m_bDebugRenderTarget;
	}
	_bool IsDebug() const {
		return m_bDebugRenderTarget;
	}

	// Shadow
	void SetShadowLevel(SHADOW_LEVEL eLevel) { 
		if (eLevel >= SHADOW_END)
			eLevel = SHADOW_OFF;
		m_eShadowLevel = eLevel; 
	}

	SHADOW_LEVEL GetShadowLevel() const { return m_eShadowLevel; }
	_bool IsActiveShadow() const { return m_eShadowLevel != SHADOW_OFF; }

	// SSAO
	void SSAOToggle() { m_bSSAO = !m_bSSAO; }
	_bool IsActiveSSAO() const { return m_bSSAO; }

	// Outline
	void OutlineToggle() { m_bOutline = !m_bOutline; };
	_bool IsActiveOutline() const { return m_bOutline; }

	// LUT
	void SetLUT(CRenderer::LUT eLUT) { 
		if (CRenderer::LUT_DEFAULT + 1 <= eLUT)
			m_eLUTFilter = CRenderer::LUT_DEFAULT;
		else
			m_eLUTFilter = eLUT;
	};
	CRenderer::LUT GetLUT() { 
		return m_eLUTFilter;
	}

	// Black & White
	void StartBlackWhite(_double TotalTime) { 
		m_fBlackWhite_TimeAcc = 0.f;
		m_fBlackWhite_TimeLimit = (_float)TotalTime;
		m_bBlackWhite = true; 
	}

	void BlackWhiteTimeAcc(_double TimeDelta) {
		m_fBlackWhite_TimeAcc += (_float)TimeDelta;
		if (m_fBlackWhite_TimeAcc >= m_fBlackWhite_TimeLimit)
		{
			m_bBlackWhite = false;
			m_fBlackWhite_TimeAcc = 0.f;
		}
	};

	_bool IsActiveBlackWhite() const { 
		return m_bBlackWhite; 
	}

	// Split RGB
	void SetSplitDesc(RGB_SPLIT_DESC tagDesc)
	{
		m_tagSplitDesc = tagDesc;
	}

	RGB_SPLIT_DESC& GetSplitDesc()
	{
		return m_tagSplitDesc;
	}

	SPLIT_DIR GetSplitDir() const {
		return m_eSplitDir;
	}

	void StartRGBSplit(SPLIT_DIR eSplitDir, _double TotalTime) {
		m_eSplitDir = eSplitDir;
		m_fRGBSplit_TimeAcc = (_float)TotalTime;
		m_fRGBSplit_TotalTime = (_float)TotalTime;
		m_bRGBSplit = true;
	}

	void RGBSpiltTimeAcc(_double TimeDelta) {
		m_fRGBSplit_TimeAcc -= (_float)TimeDelta;
		if (m_fRGBSplit_TimeAcc <= 0.f)
			m_bRGBSplit = false;
	};

	_float GetRGBSplitRatio() const { 
		return m_fRGBSplit_TimeAcc / m_fRGBSplit_TotalTime;
	}

	_bool IsActiveRGBSplit() const {
		return m_bRGBSplit; 
	}
	
	// Fade Options
	void StartFade(FADE_STATE eState, _double FadeTime) {
		m_FadeDesc.m_eFadeState = eState;
		m_FadeDesc.m_fFadeAcc = 0.f;
		m_FadeDesc.m_fFadeLimit = (_float)FadeTime;
	}

	void FadeTimeAcc(_double TimeDelta)	{
		if (m_FadeDesc.m_fFadeLimit > m_FadeDesc.m_fFadeAcc)
			m_FadeDesc.m_fFadeAcc += (_float)TimeDelta;
		else
			m_FadeDesc.m_fFadeAcc = m_FadeDesc.m_fFadeLimit;
	}

	FADE_DESC GetFadeDesc() {
		return m_FadeDesc;
	}

	_float GetFadeRatio()	{
		return m_FadeDesc.m_fFadeAcc / m_FadeDesc.m_fFadeLimit;
	}

	_bool IsFade() const {
		return m_FadeDesc.m_eFadeState != FADE_STATE::FADE_NONE;
	}

private:
	_bool m_bDebugRenderTarget = false;

	// 그림자 성능
	SHADOW_LEVEL m_eShadowLevel = SHADOW_HIGH;

	// SSAO & 외곽선
	_bool m_bSSAO = false;
	_bool m_bOutline = false;

	// 흑백연출
	_bool m_bBlackWhite = false;
	_float m_fBlackWhite_TimeAcc = 0.0f;
	_float m_fBlackWhite_TimeLimit = 0.1f;

	// RGB 분리
	RGB_SPLIT_DESC m_tagSplitDesc;
	SPLIT_DIR m_eSplitDir = SPLIT_DIR::SPLIT_DEFAULT;
	_bool m_bRGBSplit = false;
	_float m_fRGBSplit_TimeAcc = 1.0f;
	_float m_fRGBSplit_TotalTime = 1.f;

	// 필터
	CRenderer::LUT m_eLUTFilter = CRenderer::LUT_DEFAULT;

	// 페이드 인&아웃
	FADE_DESC m_FadeDesc;

};

END