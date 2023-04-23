#pragma once

#include "Base.h"

BEGIN(Engine)

struct ENGINE_DLL IAlphaSort abstract
{
public:
	_float ComputeCameraDistance(_fvector vWorldPos);

};

END