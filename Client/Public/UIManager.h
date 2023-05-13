#pragma once

#include "Base.h"

BEGIN(Client)

class CUIManager : public CBase
{
public:
	DECLARE_SINGLETON(CUIManager)

public:
	CUIManager();
	virtual ~CUIManager();
	virtual void Free() override;

public:
	void Tick(_double TimeDelta);

private:


};

END