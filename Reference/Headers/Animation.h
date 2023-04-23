#pragma once

#include "Base.h"
#include "AnimController.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ANIMINFO* pAnimInfo, class CModel_Anim* pModel);
	void Play_Animation(_double TimeDelta, CAnimController::ANIMSTATE& tState, CModel_Anim* pModel, _bool bContinue = false);

	const _uint Get_NumChannels() const { return m_iNumChannels; }

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