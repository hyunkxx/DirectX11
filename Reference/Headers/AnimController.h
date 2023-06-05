#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimController final : public CBase
{
public:
	struct ANIMSTATE {
		_double				FrameAcc = { 0.0 };
		_double				FrameRemain = { 0.0 };
		vector<_uint>		vecCurrentKeyFrame;
		_bool				isFinished = false;
		_bool				isFirstFrame = true;

		// 보간용 변수
		_bool				IsInterpolate = { false };
		vector<_float4x4>	vecLastFrameState;

		// Root 애니메이션용 변수
		_float3				vPrevRootBonePos = { 0.f, 0.f, 0.f };
		_float3				vCurRootBoneMove = { 0.f, 0.f, 0.f };

		_float4				vPrevRootBoneRot = { 0.f, 0.f, 0.f, 0.f };
		_float4				vCurRootBoneRot = { 0.f, 0.f, 0.f, 0.f };
	};

private:
	CAnimController();
	virtual ~CAnimController() = default;

public:
	const _bool Get_AnimInterpolating() const { return m_tAnimState.IsInterpolate; }
	void Set_TrackPos(_float fTrackPos)
	{
		m_tAnimState.FrameAcc = (_double)fTrackPos;

		for (auto& KeyID : m_tAnimState.vecCurrentKeyFrame)
			KeyID = 0;
	}
public:
	HRESULT Initialize(_uint iNumBones);
	HRESULT SetUp_Animation(_uint iAnimationIndex, class CModel_Anim* pModel, _bool bInterpolate, _bool bContinue);
	void	Play_Animation(_double TimeDelta, class CModel_Anim* pModel, _float4* pRotOut = nullptr, _float3* pMoveOut = nullptr, _double* pFrameAccOut = nullptr, _double* pProgressRatio = nullptr,  _bool* pFinishedOut = nullptr);
	void	Update_RibbonAnimation(_double BaseAnimTrackRatio, CModel_Anim* pModel);




private:
	_uint	m_iCurAnimation = { NO_ANIM };
	_uint	m_iPrevAnimation = { NO_ANIM };
	ANIMSTATE	m_tAnimState;

public:
	static CAnimController* Create(_uint iNumBones);
	virtual void Free();

};

END