#include "..\Public\Channel.h"

#include "Model_Anim.h"
#include "Bone.h"
#include "AnimController.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNELINFO * pChannelInfo, CModel_Anim * pModel)
{
	lstrcpy(m_szName, pChannelInfo->s_szName);

	m_iTargetBoneID = pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = pChannelInfo->s_iNumKeyFrames;
	m_KeyFrames.reserve(m_iNumKeyFrames);

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		m_KeyFrames.push_back(pChannelInfo->s_pKeyFrames[i]);
	}

	m_bApply = true;

	return S_OK;
}

void CChannel::Invalidate_Transform(_uint ChannelID, CAnimController::ANIMSTATE& tState, CModel_Anim* pModel)
{
	pModel->Get_BonePtr(m_iTargetBoneID)->Set_Apply(m_bApply);

	if (false == m_bApply)
		return;

	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	if (true == tState.isFinished || tState.FrameAcc >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	else
	{
		while (tState.FrameAcc >= m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].Time)
			++tState.vecCurrentKeyFrame[ChannelID];

		_double Ratio = (tState.FrameAcc - m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].Time) /
			(m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].Time - m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vScale), XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vRotation), XMLoadFloat4(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vRotation), (_float)Ratio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vPosition), XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vPosition), (_float)Ratio);

		//// 보간 처리
		if (true == tState.IsInterpolate)
		{
			if (tState.FrameAcc < 4.0)
			{
				if (nullptr == pModel->Get_RootBone())
				{
					_vector vDestScale, vDestRotation, vDestPosition;
					_double DestRatio = tState.FrameAcc / 4.0;

					XMMatrixDecompose(&vDestScale, &vDestRotation, &vDestPosition, XMLoadFloat4x4(&tState.vecLastFrameState[m_iTargetBoneID]));

					vScale = XMVectorLerp(vDestScale, vScale, (_float)DestRatio);
					vRotation = XMQuaternionSlerp(vDestRotation, vRotation, (_float)DestRatio);
					vPosition = XMVectorLerp(vDestPosition, vPosition, (_float)DestRatio);
				}
				else if (lstrcmp(this->Get_Name(), pModel->Get_RootBone()->Get_Name()))
				{
					_vector vDestScale, vDestRotation, vDestPosition;
					_double DestRatio = tState.FrameAcc / 4.0;

					XMMatrixDecompose(&vDestScale, &vDestRotation, &vDestPosition, XMLoadFloat4x4(&tState.vecLastFrameState[m_iTargetBoneID]));

					vScale = XMVectorLerp(vDestScale, vScale, (_float)DestRatio);
					vRotation = XMQuaternionSlerp(vDestRotation, vRotation, (_float)DestRatio);
					vPosition = XMVectorLerp(vDestPosition, vPosition, (_float)DestRatio);
				}
			}
			else
				tState.IsInterpolate = false;
		}
	}

	// Root 처리
	// RootBone 애니메이션 처리
	if (pModel->Get_RootBone() == pModel->Get_BonePtr(m_iTargetBoneID))
	{
		CBone* pRootBone = pModel->Get_RootBone();
		_vector	vCurRootBoneMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector	vCurRootBoneRot = XMQuaternionIdentity();

		if (true == tState.isFirstFrame)
		{
			vCurRootBoneMove += XMVectorSet(XMVectorGetX(vPosition) - m_KeyFrames[0].vPosition.x,
				XMVectorGetY(vPosition) - m_KeyFrames[0].vPosition.y,
				XMVectorGetZ(vPosition) - m_KeyFrames[0].vPosition.z, 0.f);

			vCurRootBoneRot = XMQuaternionMultiply(XMQuaternionNormalize(vRotation), XMQuaternionInverse(XMLoadFloat4(&m_KeyFrames[0].vRotation)));

			tState.isFirstFrame = false;
		}
		else
		{
			vCurRootBoneMove += XMVectorSet(XMVectorGetX(vPosition) - tState.vPrevRootBonePos.x,
				XMVectorGetY(vPosition) - tState.vPrevRootBonePos.y,
				XMVectorGetZ(vPosition) - tState.vPrevRootBonePos.z, 0.f);

			vCurRootBoneRot = XMQuaternionMultiply(XMQuaternionNormalize(vRotation), XMQuaternionInverse(XMLoadFloat4(&tState.vPrevRootBoneRot)));
		}
		// 이동량
		XMStoreFloat3(&tState.vCurRootBoneMove, XMVector3TransformNormal(vCurRootBoneMove, pModel->Get_LocalMatrix()));
		XMStoreFloat3(&tState.vPrevRootBonePos, vPosition);

		// 회전량 
		XMStoreFloat4(&tState.vCurRootBoneRot, vCurRootBoneRot);
		XMStoreFloat4(&tState.vPrevRootBoneRot, vRotation);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	pModel->Get_BonePtr(m_iTargetBoneID)->Set_TransformationMatrix(TransformationMatrix);

	//// RootMotion 처리용 낮은 발 높이 찾기
	//if (true == tState.bFootAltitude)
	//{
	//	if (pModel->Get_RightBottomBone() == pModel->Get_BonePtr(m_iTargetBoneID) ||
	//		pModel->Get_LeftBottomBone() == pModel->Get_BonePtr(m_iTargetBoneID))
	//	{
	//		CBone* pBone = pModel->Get_BonePtr(m_iTargetBoneID);

	//		_matrix matBottom = TransformationMatrix;
	//		while (true)
	//		{
	//			pBone = pBone->Get_ParentBone();

	//			matBottom = matBottom * XMLoadFloat4x4(&pBone->Get_TransformationMatrix());

	//			if (pBone == pModel->Get_RootBone())
	//				break;
	//		}

	//		if (true == tState.bAltitudeFirst)
	//		{
	//			tState.fAltitude = XMVectorGetY(matBottom.r[3]);
	//			tState.bAltitudeFirst = false;
	//		}
	//		else
	//			tState.fAltitude = min(tState.fAltitude, XMVectorGetY(matBottom.r[3]));
	//	}
	//}
	//else
	//	tState.fAltitude = 0.f;
}

void CChannel::Invalidate_Transform_Blending(_uint ChannelID, CAnimController::ANIMSTATE & tState, CModel_Anim * pModel)
{
	pModel->Get_BonePtr(m_iTargetBoneID)->Set_Apply(m_bApply);

	if (false == m_bApply)
		return;

	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	if (true == tState.isFinished || tState.FrameAcc >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	else
	{
		while (tState.FrameAcc >= m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].Time)
			++tState.vecCurrentKeyFrame[ChannelID];

		_double Ratio = (tState.FrameAcc - m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].Time) /
			(m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].Time - m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vScale), XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vRotation), XMLoadFloat4(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vRotation), (_float)Ratio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID]].vPosition), XMLoadFloat3(&m_KeyFrames[tState.vecCurrentKeyFrame[ChannelID] + 1].vPosition), (_float)Ratio);
	}

	//// Root 처리
	//// RootBone 애니메이션 처리
	//if (pModel->Get_RootBone() == pModel->Get_BonePtr(m_iTargetBoneID))
	//{
	//	CBone* pRootBone = pModel->Get_RootBone();
	//	_vector	vCurRootBoneMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	//	_vector	vCurRootBoneRot = XMQuaternionIdentity();

	//	if (true == tState.isFirstFrame)
	//	{
	//		vCurRootBoneMove += XMVectorSet(XMVectorGetX(vPosition) - m_KeyFrames[0].vPosition.x,
	//			XMVectorGetY(vPosition) - m_KeyFrames[0].vPosition.y,
	//			XMVectorGetZ(vPosition) - m_KeyFrames[0].vPosition.z, 0.f);

	//		vCurRootBoneRot = XMQuaternionMultiply(XMQuaternionNormalize(vRotation), XMQuaternionInverse(XMLoadFloat4(&m_KeyFrames[0].vRotation)));

	//		tState.isFirstFrame = false;
	//	}
	//	else
	//	{
	//		vCurRootBoneMove += XMVectorSet(XMVectorGetX(vPosition) - tState.vPrevRootBonePos.x,
	//			XMVectorGetY(vPosition) - tState.vPrevRootBonePos.y,
	//			XMVectorGetZ(vPosition) - tState.vPrevRootBonePos.z, 0.f);

	//		vCurRootBoneRot = XMQuaternionMultiply(XMQuaternionNormalize(vRotation), XMQuaternionInverse(XMLoadFloat4(&tState.vPrevRootBoneRot)));
	//	}
	//	// 이동량
	//	XMStoreFloat3(&tState.vCurRootBoneMove, XMVector3TransformNormal(vCurRootBoneMove, pModel->Get_LocalMatrix()));
	//	XMStoreFloat3(&tState.vPrevRootBonePos, vPosition);

	//	// 회전량 
	//	XMStoreFloat4(&tState.vCurRootBoneRot, vCurRootBoneRot);
	//	XMStoreFloat4(&tState.vPrevRootBoneRot, vRotation);
	//}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	CBone*	pTargetBone = pModel->Get_BonePtr(m_iTargetBoneID);

	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&pTargetBone->Get_DefaultTransformationMatrix()) * TransformationMatrix);


	//// 보간 처리
	if (true == tState.IsInterpolate)
	{
		if (tState.FrameAcc < 4.0)
		{
			_vector vDestScale, vDestRotation, vDestPosition;
			_double DestRatio = tState.FrameAcc / 4.0;

			XMMatrixDecompose(&vDestScale, &vDestRotation, &vDestPosition, XMLoadFloat4x4(&tState.vecLastFrameState[m_iTargetBoneID]));

			vScale = XMVectorLerp(vDestScale, vScale, (_float)DestRatio);
			vRotation = XMQuaternionSlerp(vDestRotation, vRotation, (_float)DestRatio);
			vPosition = XMVectorLerp(vDestPosition, vPosition, (_float)DestRatio);
		}
		else
			tState.IsInterpolate = false;
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);


	pTargetBone->Set_TransformationMatrix(TransformationMatrix);

}

void CChannel::Set_PoseAnim(CModel_Anim* pModel)
{
	CBone* pTargetBone = pModel->Get_BonePtr(m_iTargetBoneID);

	if (false == m_bApply)
		return;

	KEYFRAME	FirstKeyFrame = m_KeyFrames.front();
	_vector vScale = XMLoadFloat3(&FirstKeyFrame.vScale);
	_vector vRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
	_vector vPosition = XMLoadFloat3(&FirstKeyFrame.vPosition);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	pTargetBone->Set_DefaultMatrix(TransformationMatrix);
	pTargetBone->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Blend_PoseAnim(CModel_Anim * pModel, _float fRatio)
{
	CBone* pTargetBone = pModel->Get_BonePtr(m_iTargetBoneID);

	if (false == m_bApply)
	return;

	_matrix	OldMatrix = XMLoadFloat4x4(&pTargetBone->Get_DefaultTransformationMatrix());
	_vector vOldScale, vOldRotation, vOldPosition;
	XMMatrixDecompose(&vOldScale, &vOldRotation, &vOldPosition, OldMatrix);

	KEYFRAME	FirstKeyFrame = m_KeyFrames.front();

	_vector vNewScale = XMVectorLerp(vOldScale, XMLoadFloat3(&FirstKeyFrame.vScale), fRatio);
	_vector vNewRotation = XMQuaternionSlerp(vOldRotation, XMLoadFloat4(&FirstKeyFrame.vRotation), fRatio);
	_vector vNewPosition = XMVectorLerp(vOldPosition, XMLoadFloat3(&FirstKeyFrame.vPosition), fRatio);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vNewScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vNewRotation, vNewPosition);

	pTargetBone->Set_DefaultMatrix(TransformationMatrix);
	pTargetBone->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(CHANNELINFO * pChannelInfo, CModel_Anim * pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannelInfo, pModel)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
