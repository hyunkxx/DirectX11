#pragma once

#include "Base.h"
#include "AnimController.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ANIMINFO* pAnimInfo, class CModel_Anim* pModel);
	void Play_Animation(_double TimeDelta, CAnimController::ANIMSTATE& tState, CModel_Anim* pModel);

	const _tchar* Get_Name()
	{
		return m_szName;
	}

	_double Get_Duration()
	{
		return m_Duration;
	}

	_double Get_TicksPerSecond()
	{
		return m_TicksPerSecond;
	}

	void Set_TicksPerSecond(_double TicksPerSecond)
	{
		m_TicksPerSecond = TicksPerSecond;
	}

	const _uint Get_NumChannels() const { return m_iNumChannels; }
	vector<class CChannel*>& Get_Channels();

private:
	_tchar	m_szName[MAX_PATH] = TEXT("");
	_double	m_Duration = { 0.0 };
	_double m_TicksPerSecond = { 0.0 };

	//
	//_double m_TimeAcc = { 0.0 };
	//_bool	m_isFinished = { false };


private:
	_uint	m_iNumChannels = { 0 };
	vector<class CChannel*> m_Channels;

public:
	static CAnimation* Create(ANIMINFO* pAnimInfo, class CModel_Anim* pModel);
	virtual void Free() override;
};

END