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

	m_pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pMainTransform->Get_WorldMatrixInverse());
	m_pPipeLine->Set_Transform(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	//if (pInstance->Input_KeyState_Custom(DIK_LSHIFT) == KEY_STATE::HOLD)
	//{
	//	m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed * 2.f;
	//}
	//else
	//{
	//	m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed * 0.5f;
	//}

}

void CCamera::LateTick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pPipeLine);
}
