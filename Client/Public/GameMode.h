#pragma once

#include "Base.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Renderer.h"

class CGameMode final : public CBase
{
public:
	DECLARE_SINGLETON(CGameMode)

public:
	explicit CGameMode() = default;
	virtual ~CGameMode() = default;
	virtual void Free() override;

public:
	void SetCurrentLevel(_uint nCurrentLevel) { m_nCurrentLevel = nCurrentLevel; }
	_uint GetCurrentLevel() const { return m_nCurrentLevel; };

private:
	_uint m_nCurrentLevel = 0;

};