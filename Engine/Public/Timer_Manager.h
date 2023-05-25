#pragma once

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
	enum TIME_TYPE { TIME_SCALEDOWN, TIME_SCALEUP, TIME_DEFAULT };

private:
	explicit CTimer_Manager() = default;
	virtual ~CTimer_Manager() = default;

public:
	void ScaleUpdate(_bool bValue) { m_bScaleUpdate = bValue; }
	_bool IsScaleUpdate() const { return m_bScaleUpdate; }
	_double GetTimeScale() const { return m_fTimeScale; }
	void SetTimeScaleDesc(_float fDuration, _float fTargetTime) {
		m_bSlowDown = true;
		m_fDuration = fDuration;
		m_fTargetTimeScale = fTargetTime;
	}
	void TimeScaleDown(_double TimeDelta);

	HRESULT	CreateTimer(const _tchar* pTimerTag);
	_float	GetTimer(const _tchar* pTimerTag);
	void	SetTimer(const _tchar* pTimerTag);
	
private:
	CTimer*	FindTimer(const _tchar* pTimerTag);

public:
	virtual	void Free(void) override;
	      
private:
	unordered_map<const _tchar*, CTimer*> m_umapTimers;

	_bool m_bScaleUpdate = false;
	_bool m_bSlowDown = false;
	_float m_fTimeAcc = 0.f;
	_float m_fTimeScale = 1.f;
	_float m_fTargetTimeScale = 1.f;
	_float m_fDuration = 1.f;
};

END
