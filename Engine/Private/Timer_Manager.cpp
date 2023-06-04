#include "Timer_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimer_Manager)

_float Engine::CTimer_Manager::GetTimer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::SetTimer(const _tchar * pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update();
}

CTimer* Engine::CTimer_Manager::FindTimer(const _tchar* pTimerTag)
{
	auto iter = find_if(m_umapTimers.begin(), m_umapTimers.end(), CTagFinder(pTimerTag));
	if (iter == m_umapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::TimeScaleDown(_double TimeDelta)
{
	static _bool bScaleDownFinish = false;

	_vector vCurTime = XMVectorSet(m_fTimeScale, m_fTimeScale, m_fTimeScale, 1.f);
	_vector vSlowTime = XMVectorSet(m_fTargetTimeScale, m_fTargetTimeScale, m_fTargetTimeScale, 1.f);

	if(bScaleDownFinish)
	{
		m_fTimeAcc += (_float)TimeDelta;
		if (m_fTimeAcc > m_fDuration)
		{
			m_fTimeAcc = 0.f;
			m_bSlowDown = false;
		}
	}

	if (m_bSlowDown)
	{
		vCurTime = XMVectorLerp(vCurTime, vSlowTime, (_float)TimeDelta * m_fLerpTime);
		m_fTimeScale = XMVectorGetX(vCurTime);

		if (abs(m_fTimeScale - XMVectorGetX(vSlowTime)) <= 0.01f)
			bScaleDownFinish = true;
	}
	else
	{
		vCurTime = XMVectorLerp(vCurTime, XMVectorSet(1.f, 1.f, 1.f, 1.f), (_float)TimeDelta * m_fLerpTime);
		m_fTimeScale = XMVectorGetX(vCurTime);
		if (m_fTimeScale >= 0.99f)
		{
			m_fLerpTime = 5.f;
			m_fDuration = 1.f;
			m_fTimeScale = 1.f;
			m_fTargetTimeScale = 1.f;
			m_bScaleUpdate = false;
			bScaleDownFinish = false;
		}
	}

}

HRESULT Engine::CTimer_Manager::CreateTimer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);

	assert(nullptr == pTimer);
	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();

	assert(nullptr != pTimer);
	if (nullptr == pTimer)
		return E_FAIL;

	m_umapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void Engine::CTimer_Manager::Free(void)
{
	for (auto& Pair : m_umapTimers)
		Safe_Release(Pair.second);

	m_umapTimers.clear();
}
