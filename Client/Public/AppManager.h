#pragma once

#include "Base.h"

class CAppManager final : public CBase
{
	DECLARE_SINGLETON(CAppManager)

private:
	explicit CAppManager() = default;
	virtual ~CAppManager() = default;

public:
	void SetTitle(wstring strTitle);

public:
	virtual void Free() override;

private:
	std::wstring m_strTitleText;

};

