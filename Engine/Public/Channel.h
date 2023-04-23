#pragma once

#include "Base.h"

#include "AnimController.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CHANNELINFO* pChannelInfo, class CModel_Anim* pModel);
	void CChannel::Invalidate_Transform(_uint ChannelID, CAnimController::ANIMSTATE& tState, class CModel_Anim* pModel);

private:
	_tchar			m_szName[MAX_PATH] = TEXT("");
	_uint			m_iTargetBoneID = { 0 };

private:
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

//private:
//	_uint m_iCurrentKeyFrame = { 0 };

public:
	static CChannel* Create(CHANNELINFO* pChannelInfo, class CModel_Anim* pModel);
	virtual void Free() override;
};

END