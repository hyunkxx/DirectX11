#include "..\Public\Animation.h"

#include "Channel.h"
#include "Model_Anim.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ANIMINFO * pAnimInfo, CModel_Anim * pModel)
{
	lstrcpy(m_szName, pAnimInfo->s_szName);
	m_Duration = pAnimInfo->s_Duration;
	m_TicksPerSecond = pAnimInfo->s_TicksPerSecond;
	m_iNumChannels = pAnimInfo->s_iNumChannels;
	m_Channels.reserve(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(&pAnimInfo->s_pChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Play_Animation(_double TimeDelta, CAnimController::ANIMSTATE& tState, CModel_Anim* pModel)
{
	tState.FrameAcc += m_TicksPerSecond * TimeDelta;

	if (tState.FrameAcc > m_Duration)
	{
		tState.isFinished = true;
		tState.FrameRemain = tState.FrameAcc - m_Duration;
	}

	_uint iChannelID = 0;
	for (auto& pChannel : m_Channels)
	{
		pChannel->Invalidate_Transform(iChannelID++, tState, pModel);
	}
}

void CAnimation::Play_Animation_Blending(_double TimeDelta, CAnimController::ANIMSTATE & tState, CModel_Anim * pModel)
{
	tState.FrameAcc += m_TicksPerSecond * TimeDelta;

	if (tState.FrameAcc > m_Duration)
	{
		tState.isFinished = true;
		tState.FrameRemain = tState.FrameAcc - m_Duration;
	}

	_uint iChannelID = 0;
	for (auto& pChannel : m_Channels)
	{
		pChannel->Invalidate_Transform_Blending(iChannelID++, tState, pModel);
	}
}

void CAnimation::Update_RibbonAnimation(_double BaseAnimTrackRatio, CAnimController::ANIMSTATE & tState, CModel_Anim * pModel)
{
	
	tState.FrameAcc = m_Duration * BaseAnimTrackRatio;

	_uint iChannelID = 0;
	for (auto& pChannel : m_Channels)
	{
		pChannel->Invalidate_Transform(iChannelID++, tState, pModel);
	}
}

void CAnimation::Set_PoseAnim(CModel_Anim* pModel)
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Set_PoseAnim(pModel);
	}
}

void CAnimation::Blend_PoseAnim(CModel_Anim * pModel, _float fRatio)
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Blend_PoseAnim(pModel, fRatio);
	}
}

vector<class CChannel*>& CAnimation::Get_Channels()
{
	return m_Channels;
}

CChannel * CAnimation::Get_Channel(_tchar * pChannelTag)
{
	auto& iter = find_if(m_Channels.begin(), m_Channels.end(), [&](CChannel* pChannel)
	{
		return !lstrcmp(pChannel->Get_Name(), pChannelTag);
	});

	if (iter == m_Channels.end())
		return nullptr;

	return *iter;
}

CAnimation * CAnimation::Create(ANIMINFO * pAnimInfo, CModel_Anim * pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimInfo, pModel)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

