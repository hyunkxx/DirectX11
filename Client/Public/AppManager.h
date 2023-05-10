#pragma once

#include "Base.h"

BEGIN(Client)

class CAppManager final : public CBase
{
	DECLARE_SINGLETON(CAppManager)

private:
	explicit CAppManager() = default;
	virtual ~CAppManager() = default;

public:
	void SetTitle(wstring strTitle);

	void ResetLoadRatio() {
		m_fLoading = 0.f;
	}

	void LoadRatio(_float fValue) {
		m_fLoading = fValue;
	};

	_float GetCurrentLoadRatio() const {
		return m_fLoading;
	}

public:
	virtual void Free() override;

private:
	std::wstring m_strTitleText;
	_float m_fLoading = 0.f;

};

END