#pragma once

#include "Level.h"

BEGIN(Engine)

class ENGINE_DLL CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	explicit CLevel_Manager() = default;
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, CLevel* pCurrentLevel);
	void Tick_Level(_double TimeDelta);
	void RenderLevelUI();
public:
	virtual void Free() override;

private:
	_bool	m_bTickStart = false;
	_uint	m_iLevelIndex = { 0 };
	CLevel*	m_pCurrentLevel = { nullptr };

};

END