#pragma once

#include "Base.h"

BEGIN(Client)

class CInGameLoader final : public CBase
{
public:
	explicit CInGameLoader();
	virtual ~CInGameLoader() = default;
	virtual void Free() override;

};

END