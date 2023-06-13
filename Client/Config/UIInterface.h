#pragma once

#include "Base.h"

BEGIN(Client)

struct IOnClick abstract
{
	virtual void OnClick() = 0;
};

struct IActivate abstract
{
	virtual void SetActive(_bool bValue) = 0;
	virtual void SetRender(_bool bValue) = 0;
};

enum COLOR_TYPE { NORMAL, TONEDOWN };

END