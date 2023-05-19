#include "..\Public\Camera.h"
#include "PipeLine.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine{ CPipeLine::GetInstance() }
	, m_bUse{ false }
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
	, m_bUse{ false }
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	m_pMainTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pMainTransform)
		return E_FAIL;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
	m_pMainTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	return S_OK;
}

void CCamera::Start()
{
}

void CCamera::Tick(_double TimeDelta)
{
	if (false == m_bUse)
		return;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (m_bShakeActive)
	{
		switch (m_eShakeType)
		{
		case WAVE:
			shakeWave(TimeDelta);
			break;
		case VIBRATION:
			shakeVibration(TimeDelta);
			break;
		}
	}
	
	m_pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pMainTransform->Get_WorldMatrixInverse());
	m_pPipeLine->Set_Transform(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

}

void CCamera::LateTick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::StartWave()
{
	if (m_bShakeActive)
		return;

	m_bShakeActive = true;

	m_eShakeType = WAVE;
	m_SakeDesc.fPower = 2.5f;
	m_SakeDesc.fSpeed = 10.f;
	m_SakeDesc.fDuration = 1.f;

	m_fCurShakeAcc = 0.f;
	m_fCurTimeAcc = 0.f;
}

void CCamera::StartWave(SHAKE_DESC tagShakeDesc)
{
	if (m_bShakeActive)
		return;

	m_bShakeActive = true;

	m_eShakeType = WAVE;
	m_SakeDesc = tagShakeDesc;

	m_fCurShakeAcc = 0.f;
	m_fCurTimeAcc = 0.f;
}

void CCamera::StartVibration()
{
	if (m_bShakeActive)
		return;

	m_bShakeActive = true;
	m_eShakeType = VIBRATION;

	m_fCurShakeAcc = 0.f;
	m_fCurTimeAcc = 0.f;
	m_fVibeRange = 5.f;
	m_fVibeDuration = 0.5f;
}

void CCamera::StartVibration(_float fRange, _float fDuration)
{
	if (m_bShakeActive)
		return;

	m_bShakeActive = true;
	m_eShakeType = VIBRATION;

	m_fCurShakeAcc = 0.f;
	m_fCurTimeAcc = 0.f;
	m_fVibeRange = fRange;
	m_fVibeDuration = fDuration;
}

void CCamera::shakeWave(_double TimeDelta)
{
	_vector vCamPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);

	vCamPos = XMVectorSetY(vCamPos, m_CameraDesc.vEye.y + sin(m_SakeDesc.fPower * m_fCurShakeAcc) * powf(m_SakeDesc.fPower, m_SakeDesc.fDuration - m_fCurShakeAcc));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCamPos);

	m_fCurShakeAcc += (_float)TimeDelta *  m_SakeDesc.fSpeed;
	m_fCurTimeAcc += (_float)TimeDelta;
	if (m_fCurTimeAcc >= m_SakeDesc.fDuration)
		resetShakeTimer();
}

void CCamera::shakeVibration(_double TimeDelta)
{
	_vector vCamPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamRight = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_RIGHT));
	_vector vCamRUp = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_UP));

	static _float iRandomX;
	static _float iRandomY;
	
	m_fVibeTimeAcc += (_float)TimeDelta;
	if (m_fVibeTimeAcc >= 0.01f)
	{
		m_fVibeTimeAcc = 0.f;
		iRandomX = _float(rand() % (_int)m_fVibeRange - (m_fVibeRange * 0.5f)) * (1.f - (m_fCurTimeAcc / m_fVibeDuration));
		iRandomY = _float(rand() % (_int)m_fVibeRange - (m_fVibeRange * 0.5f)) * (1.f - (m_fCurTimeAcc / m_fVibeDuration));
	}

	_vector vShakePos = vCamPos + vCamRight * iRandomX;
	vShakePos = vShakePos + vCamRUp * iRandomY;

	vCamPos = XMVectorLerp(vCamPos, vShakePos, (_float)TimeDelta * 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCamPos);

	m_fCurTimeAcc += (_float)TimeDelta;
	m_fCurShakeAcc += (_float)TimeDelta;
	if (m_fCurTimeAcc >m_fVibeDuration)
		resetShakeTimer();
}

void CCamera::resetShakeTimer()
{
	m_bShakeActive = false;
	m_fCurShakeAcc = 0.f;
	m_fCurTimeAcc = 0.f;
	m_fVibeTimeAcc = 0.f;
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pPipeLine);
}
