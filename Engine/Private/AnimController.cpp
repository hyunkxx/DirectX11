#include "..\Public\AnimController.h"

#include "Model_Anim.h"
#include "Animation.h"

CAnimController::CAnimController()
{
}

HRESULT CAnimController::Initialize(_uint iNumBones)
{
	m_tAnimState.vecLastFrameState.reserve(iNumBones);

	return S_OK;
}

HRESULT CAnimController::SetUp_Animation(_uint iAnimationIndex, CModel_Anim * pModel, _bool bInterpolate, _bool bContinue)
{
	m_iPrevAnimation = m_iCurAnimation;
	m_iCurAnimation = iAnimationIndex;

	// 이전 애니메이션 종료 상태 저장

	if (true == bInterpolate)
	{
		if (NO_ANIM != m_iPrevAnimation)
		{
			m_tAnimState.IsInterpolate = true;
			m_tAnimState.vecLastFrameState.resize(pModel->Get_NumBones());
			CAnimation* pPrevAnim = pModel->Get_Animation(m_iPrevAnimation);

			for (_uint i = 0; i < pModel->Get_NumBones(); ++i)
			{
				m_tAnimState.vecLastFrameState[i] = pModel->Get_BonePtr(i)->Get_TransformationMatrix();
			}
		}
	}

	// 애니메이션에 맞춰 초기값 설정
	m_tAnimState.vecCurrentKeyFrame.clear();
	m_tAnimState.vecCurrentKeyFrame.resize(pModel->Get_Animation(m_iCurAnimation)->Get_NumChannels());

	// 0초기화
	if (true == bContinue)
		m_tAnimState.FrameAcc = m_tAnimState.FrameRemain;
	else
		m_tAnimState.FrameAcc = 0.0;

	m_tAnimState.FrameRemain = 0.0;
	m_tAnimState.isFinished = false;
	ZeroMemory(&m_tAnimState.vCurRootBoneMove, sizeof(_float3));
	ZeroMemory(&m_tAnimState.vPrevRootBonePos, sizeof(_float3));

	m_tAnimState.vCurRootBoneRot = _float4(0.f, 0.f, 0.f, 1.f);
	m_tAnimState.vPrevRootBoneRot = _float4(0.f, 0.f, 0.f, 1.f);

	m_tAnimState.isFirstFrame = true;

	return S_OK;
}

void CAnimController::Play_Animation(_double TimeDelta, CModel_Anim * pModel, _float4* pRotOut, _float3* pMoveOut, _double* pFrameAccOut, _bool* pFinishedOut)
{
	if (true == m_tAnimState.isFinished)
		return;

	CAnimation* pAnim = pModel->Get_Animation(m_iCurAnimation);

	pAnim->Play_Animation(TimeDelta, m_tAnimState, pModel);

	if (nullptr != pFrameAccOut)
	{
		*pFrameAccOut = m_tAnimState.FrameAcc;
	}
	if (nullptr != pFinishedOut)
	{
		*pFinishedOut = m_tAnimState.isFinished;
	}

	CBone* pRootBone = pModel->Get_RootBone();
	if (nullptr != pRootBone)
	{
		_vector vScale, vRot, vTrans;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, XMLoadFloat4x4(&pRootBone->Get_TransformationMatrix()));
		vRot = XMQuaternionIdentity();
		vTrans = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		pRootBone->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vTrans));
	}

	

	if (nullptr != pMoveOut)
	{
		if (nullptr != pRotOut)
		{
			*pRotOut = m_tAnimState.vCurRootBoneRot;
			XMStoreFloat3(pMoveOut, XMVector3TransformNormal(XMLoadFloat3(&m_tAnimState.vCurRootBoneMove), XMMatrixRotationQuaternion(XMQuaternionMultiply(XMLoadFloat4(&m_tAnimState.vCurRootBoneRot), XMLoadFloat4(&m_tAnimState.vPrevRootBoneRot)))));
		}
		else
			*pMoveOut = m_tAnimState.vCurRootBoneMove;
	}
		

	

	return ;
}

CAnimController * CAnimController::Create(_uint iNumBones)
{
	CAnimController* pInstance = new CAnimController();

	if (FAILED(pInstance->Initialize(iNumBones)))
	{
		MSG_BOX("Failed to create : CAnimController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimController::Free()
{


}

