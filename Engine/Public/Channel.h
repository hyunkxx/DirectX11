#pragma once

#include "Base.h"

#include "AnimController.h"

BEGIN(Engine)

class ENGINE_DLL CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CHANNELINFO* pChannelInfo, class CModel_Anim* pModel);
	void CChannel::Invalidate_Transform(_uint ChannelID, CAnimController::ANIMSTATE& tState, class CModel_Anim* pModel);

	_uint Get_NumKeyFrames()
	{
		return m_iNumKeyFrames;
	}

	_uint Get_TargetBoneID()
	{
		return m_iTargetBoneID;
	}

	const _tchar* Get_Name()
	{
		return m_szName;
	}

	vector<KEYFRAME>& Get_KeyFrames()
	{
		return m_KeyFrames;
	}

	void Set_Apply(_bool bApply)
	{
		m_bApply = bApply;
	}

private:
	_tchar			m_szName[MAX_PATH] = TEXT("");
	_uint			m_iTargetBoneID = { 0 };

	// AnimSet Àü¿ë
	_bool			m_bApply = { false };

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