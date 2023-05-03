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

public:
	explicit CRenderSetting() = default;
	virtual ~CRenderSetting() = default;
	virtual void Free() override;

public:
	void SetShadowLevel(SHADOW_LEVEL eLevel) { 
		if (eLevel >= SHADOW_END)
			eLevel = SHADOW_OFF;

		m_eShadowLevel = eLevel; 
	}
	SHADOW_LEVEL GetShadowLevel() const { return m_eShadowLevel; }
	_bool IsActiveShadow() const { return m_eShadowLevel != SHADOW_OFF; }

	void SSAOToggle() { m_bSSAO = !m_bSSAO; }
	_bool IsActiveSSAO() const { return m_bSSAO; }

	void OutlineToggle() { m_bOutline = !m_bOutline; };
	_bool IsActiveOutline() const { return m_bOutline; }

	void SetLUT(CRenderer::LUT eLUT) { 
		if (CRenderer::LUT_DEFAULT + 1 <= eLUT)
			m_eLUTFilter = CRenderer::LUT_DEFAULT;
		else
			m_eLUTFilter = eLUT;
	};
	CRenderer::LUT GetLUT() { return m_eLUTFilter; }

private:
	SHADOW_LEVEL m_eShadowLevel = SHADOW_OFF;

	_bool m_bSSAO = false;
	_bool m_bOutline = false;

	CRenderer::LUT m_eLUTFilter = CRenderer::LUT_DEFAULT;
};

END